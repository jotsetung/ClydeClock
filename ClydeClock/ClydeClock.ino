/*
 * ClydeClock
 * ----------
 *
 * Provides some basic Clyde usage and allows to use Clyde as an
 * "alarm clock"
 *
 * Bases on the ClydeDev from Erin RobotGrrl (http://robotgrrl.com)
 * Library requirements:
 * - ClydeDev
 * - Streaming
 * - MPR121 & I2Cdev
 * - Time
 * - TimeAlarms
 * October, 2014
 *
 */

#include <Wire.h>
#include <MPR121.h>
#include <Streaming.h>

#include <ClydeDev.h>
#include <ClydeTouchyFeely.h>
#include <ClydeAfraidDark.h>

///////////////////////////
// DEBUGGING STUFF:
//#define BASIC_DEBUG      // enable debugging of the Basic.ino
//#define EYE_DEBUG
//#define AFRAID_DARK_DEBUG
//#define TOUCHY_FEELY_DEBUG
//#define MIKE_DEBUG
#define PROXYSENS_DEBUG
//#define SPEAK_DEBUG
//////////////////////////

//////////////////////////
// ENABLE VARIOUS ADDONS:
#define ENABLE_CLOCK    // to enable the "clock" stuff...
//#define ENABLE_EYE      // enable the eye.
#define ENABLE_MIKE     // to enable microphone relates things
//#define ENABLE_SPEAK    // enable sound output.
#define ENABLE_PROXYSENS  // enable the HC-SR04 based proximity sensor.
//////////////////////////


// -- clyde dev
ClydeDev clyde = ClydeDev();

// -- touchy feely module in module port 1
ClydeTouchyFeely touchyfeely = ClydeTouchyFeely(1);
boolean tf_enabled = false;
static const uint8_t TOUCH_LEVEL = 4*0x0A;        // touch threshold of mpr121m was 0x06
static const uint8_t RELEASE_LEVEL = 0x0A;      // release threshold of mpr121 (0x0A) (was 0x04)
uint32_t touch_array_last_touch_time = 0;
uint8_t touch_array_touched_leg[ 8 ] = {0,0,0,0,0,0,0,0};
uint16_t TOUCH_TRIGGER_DELAY = 600;             // trigger an action if the last
uint8_t touch_counter = 0;                      // keep track of the total number of touches without pause
uint8_t max_touch_counter_reset = 8;           // if we have more than this touches without a pause, then it's most likely the touchy feely sensor needs to be reset... thus calling reset.
boolean touchyfeely_auto = false;    // do auto confit on reset.


// afraid of the dark module in module port 2
ClydeAfraidDark afraiddark = ClydeAfraidDark(2);
boolean ad_enabled = false;
boolean currently_pressed = false;

// that's my global variables.
uint32_t fadetime [1] = {1000};         // default time for color fading.
static const uint8_t max_steps = 12;    // max number of allowed steps; need this to pre-allocate the memory.
// for the white light:
uint8_t wl_current_step = 0;            // current brightness step for the white light.
uint8_t wl_number_steps = 0;            // the number of steps of the cycle.
uint32_t wl_step_start = millis();      // when was the step started?
uint8_t wl_step_start_intensity = 0;    // the intensity at the start of a cycle step.
uint8_t wl_intensity = 0;               // the current brightness of the white light.
uint8_t wl_previous_intensity = 0;
uint8_t wl_step_intensities[max_steps]; // the array with the brightness values to cycle through.
uint32_t wl_step_durations[max_steps];  // the duration (ms) for each brightness step.
// for the RGB light:
uint8_t current_step = 0;               // current step for the cycle through RGB values.
uint8_t number_steps = 0;               // the number of steps.
uint32_t step_start = millis();         // the time when the cycle step started.
uint8_t step_start_r = 0;               // the red intensity at the beginning of a cycle step.
uint8_t step_start_g = 0;               // the green intensity at the beginning of a cycle step.
uint8_t step_start_b = 0;               // the blue intensity at the beginning of a cycle step.
uint8_t step_colors[max_steps*3];       // the array with the RGB values to cycle through.
uint32_t step_durations[max_steps];     // the array with the durations for each cycle step.
// variables related to the color select cycle.
boolean cycle_rgb = false;   // if that's true we're cycling.
boolean cycle_wl = false;    // if that's true we're cycling.
uint32_t cycle_last_update_ms = 0;  // keep track of the last time we updated the light.
uint32_t CYCLE_SLEEP_MS = 50;       // ms to sleep between cycle steps.
boolean wl_increasing = false;  // increment or decrement the white light intensity.


// TouchyFeely related:
//uint8_t leg_switch = 2;  // set this to a value from 0-5 if you want to use a leg as the light switch instead of the eye... e.g. if the eye is buggy, like the one of my Clyde.
char leg_switch[1] = "";
// Some pre-defined colors (from R's RColorBrewer package).
uint8_t COLORBREWER_RED [ 3 ] = { 228, 26, 28 };
uint8_t COLORBREWER_BLUE [ 3 ] = { 55, 126, 184 };
//uint8_t COLORBREWER_GREEN [ 3 ] = { 77, 175, 74 };
uint8_t COLORBREWER_GREEN [ 3 ] = { 0, 255, 0 };
uint8_t COLORBREWER_PURPLE [ 3 ] = { 152, 78, 163 };
uint8_t COLORBREWER_ORANGE [ 3 ] = { 255, 127, 0 };
uint8_t COLORBREWER_PINK [ 3 ] = { 240, 2, 127 };  // not really in RColorBrewer...

void setup() {

  // initialize variables:
  memset((void*)&wl_step_intensities[0], 0, sizeof( uint8_t )*max_steps );
  memset((void*)&wl_step_durations[0], 0, sizeof( uint32_t )*max_steps );
  memset((void*)&step_colors[0], 0, sizeof( uint8_t )*max_steps*3 );
  memset((void*)&step_durations[0], 0, sizeof( uint32_t )*max_steps );

  // init leg switch...
  //strcat( leg_switch, "3" );
  strcat( leg_switch, "9" ); /// <--- set here the leg switch!!!

  Serial.begin(9600);
  // comment the line below if you don't want serial communication.
  //while(!Serial);

  // initialize lights
  wl_current_step=0;
  wl_number_steps=0;
  current_step=0;
  number_steps=0;

  // -- clyde!
  //clyde.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  clyde.init();

#ifdef ENABLE_EYE
  clyde.setEyePressedHandler(clydeEyePressed);
  clyde.setEyeReleasedHandler(clydeEyeReleased);
#endif
  // -- touchy feely module
  //touchyfeely.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  tf_enabled = touchyfeely.init();

  if(tf_enabled) {
    touchyfeely.reset( touchyfeely_auto, TOUCH_LEVEL, RELEASE_LEVEL );
    // these handlers are defined in TouchyFeely.ino
    touchyfeely.setTouchedHandler(clydeTouched);
    touchyfeely.setReleasedHandler(clydeReleased);
    touchyfeely.setDetectedHandler(clydeDetected);
  }

  // -- afraid of the dark module
  //afraiddark.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  ad_enabled = afraiddark.init();

#ifdef ENABLE_CLOCK
  // initialize time, alarms etc.
  initializeClock();
#endif

#ifdef ENABLE_SPEAK
  // initialize the audio out.
  initializeSpeak();
  testSong();
#endif

#ifdef ENABLE_PROXYSENS
  initializeProxySens();
#endif
  // waits for 5 seconds to properly start all modules.
  delay(5000);

  Serial << "Hello! :3" << endl;

  // initialize lights.
  clyde.setLight( wl_intensity );
  clyde.setEyeRGB( step_start_r, step_start_g, step_start_b );
}

// the main loop of clyde...
void loop() {

  clyde.update();
  ///////
  // all the rgb and white light related things:
  // check if we are in some color cycle and if so update the colors accordingly
  cycleThroughRGBColors();
  // the same for the white light
  wl_previous_intensity = wl_intensity;  // just remember the white light before fading
  fadeWhiteLight();
  updateCycle();     // in case we are in a color select cycle, serve that.

  if(tf_enabled) touchyfeely.update();

  if(ad_enabled) {
    afraiddark.update();
    //checkForDarkness();
  }

#ifdef ENABLE_CLOCK
  updateClock();
#endif

#ifdef ENABLE_MIKE
  updateMike();
#endif

#ifdef ENABLE_SPEAK
  updateSpeak();
#endif

#ifdef ENABLE_PROXYSENS
  updateProxySens();
#endif

  // evaluate touches.
  evalTouchTimeArray();
}



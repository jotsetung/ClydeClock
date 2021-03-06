// this file contains some basic functions.

// similar as cycleRGB, just for the white light.
// require global parameters:
// need:
//  - time when the step started   : wl_step_start
//  - duration of the step.        : wl_step_durations array
//  - intensity at the step_start  : wl_step_start_intensity
//  - intensity at the end of step : wl_step_intensities array
// calculate:
//  - difference in intensity / step duration: that's the change of intensity per ms
//  - set the intensity: current time - step start time * difference in intensity
// NOTE: eventually calculate the "per step intensity increment" outside this function.
void fadeWhiteLight(){
  if( wl_current_step == 0 || wl_current_step > wl_number_steps ){
    wl_current_step = 0;
    return;   // return if we're not supposed to fade the light or are finished with fading.
  }
  //uint32_t now = millis();
  uint32_t time_in_cycle_step = millis() - wl_step_start;
  if( time_in_cycle_step==0 )
    time_in_cycle_step = 1;
  // check if we're still in this step or if we have to move to the next one.
  if( time_in_cycle_step > wl_step_durations[ ( wl_current_step - 1 ) ] ){
    wl_current_step++;
    // just make sure we still have some steps...
    if( wl_current_step > wl_number_steps ){
#ifdef BASIC_DEBUG
      Serial << "Basic: fadeWhiteLight done with the cycle b" << endl;
#endif
      wl_current_step=0;
      return;
    }
    wl_step_start = millis();
    wl_step_start_intensity = wl_intensity;
    time_in_cycle_step = 1;  // set the time in step to 1 ms
  }
  // calculate the increment in intensity per ms.
  float intensity_increment = (float)( wl_step_intensities[ ( wl_current_step - 1 ) ] - wl_step_start_intensity ) / (float)wl_step_durations[ ( wl_current_step - 1 ) ]; // float or would int16_t also do?
  // calculate the actual intensity...
  wl_intensity = wl_step_start_intensity + time_in_cycle_step * intensity_increment;
#ifdef BASIC_DEBUG
  Serial << "fadeWhiteLight: intensity_increment: " << intensity_increment << " calculated light: " << wl_intensity << endl;
#endif
  // make sure we're not out of range... can that happen for a uint_8???
  if( wl_intensity > 255 ){
    wl_intensity = 255;
  }
  if( wl_intensity < 0 ){
    wl_intensity = 0;
  }
  clyde.setLight( wl_intensity );
}

//void startFadeWhiteLight(uint8_t *intens, uint16_t *durations ){
void startFadeWhiteLight(uint8_t intens[], uint32_t durations[], uint8_t no_steps ){
  wl_current_step = 1; // initialize the step, so function fadeWhiteLight will actually fade.
  wl_step_start_intensity = wl_intensity;
  //wl_number_steps = sizeof(durations)/sizeof(durations[0]);  // define the number of steps we will go through.
  wl_number_steps = no_steps;
  wl_step_start = millis();
  // copy the contents of the arrays to the global variables wl_step_durations and wl_step_intensities.
  for( int i=0; i < wl_number_steps; i++ ){
    wl_step_intensities[ i ] = intens[ i ];
    wl_step_durations[ i ] = durations[ i ];
  }
#ifdef BASIC_DEBUG
  Serial << "Basic: startFadeWhiteLight called: no_steps: " << wl_number_steps << " step_start: " << wl_step_start_intensity << " step_intensity[0]: " << wl_step_intensities[0] << " intens[0]: " << intens[0] << endl;
#endif
}

// cycles through a pre-defined set of colors in a pre-defined time.
// functionality is similar to fadeWhiteLight above...
// this should be called by the "update" function.
// need:
//  - time when the step started   : step_start
//  - duration of the step.        : step_durations array
//  - intensity at the step_start  : step_start_r, g, b
//  - intensity at the end of step : step_colors array
// calculate:
//  - difference in intensity / step duration: that's the change of intensity per ms
//  - set the intensity: current time - step start time * difference in intensity
void cycleThroughRGBColors( ){
  // if the step if 0 we don't do anything. everything > 0 means we're in a cycle.
  // we also return if the current step is larger than we have colors/steps in the cycle.
  // btw... sizeof( step_durations ) / sizeof( step_durations[ 0 ] ) is the C style of getting the length of an array.
  if( current_step==0 || current_step > number_steps ){
    current_step=0;
    return;
  }
  uint32_t time_in_cycle_step = millis() - step_start;  // needs that step_start is defined...
  // check if we're still in this step or if we have to move to the next one.
  if( time_in_cycle_step > step_durations[ ( current_step - 1 ) ] ){
    // move on to the next step.
    current_step++;
    // just make sure we still have some steps...
    if( current_step > number_steps ){
#ifdef BASIC_DEBUG
      Serial << "Basic: cycleThroughRGBColors done with the cycle b" << endl;
#endif
      current_step=0;
      return;
    }
    step_start = millis();
    step_start_r = clyde.current_colour[ 0 ];
    step_start_g = clyde.current_colour[ 1 ];
    step_start_b = clyde.current_colour[ 2 ];
    time_in_cycle_step = 1;  // set the time in step to 1 ms
  }
  // calculate the color increment per ms. could be placed outside.
  float increment_r = (float)( step_colors[ ( current_step - 1 )*3 ] - step_start_r ) / (float)step_durations[ ( current_step - 1 ) ]; // float or would int16_t also do?
  float increment_g = (float)( step_colors[ ( current_step - 1 )*3 + 1 ] - step_start_g ) / (float)step_durations[ ( current_step - 1 ) ]; // float or would int16_t also do?
  float increment_b = (float)( step_colors[ ( current_step - 1 )*3 + 2 ] - step_start_b ) / (float)step_durations[ ( current_step - 1 ) ]; // float or would int16_t also do?
  // calculate the actual colors...
  uint8_t r = step_start_r + time_in_cycle_step * increment_r;
  uint8_t g = step_start_g + time_in_cycle_step * increment_g;
  uint8_t b = step_start_b + time_in_cycle_step * increment_b;
  // debug...
#ifdef BASIC_DEBUG
  Serial << "cycleThroughRGBColors: increment_r " << increment_r << " r " << r << endl;
  Serial << "cycleThroughRGBColors: increment_g " << increment_g << " g " << g << endl;
  Serial << "cycleThroughRGBColors: increment_b " << increment_b << " b " << b << endl;
#endif
  if( r > 255 )
    r = 255;
  if( r < 0 )
    r = 0;
  if( g > 255 )
    g = 255;
  if( g < 0 )
    g = 0;
  if( b > 255 )
    b = 255;
  if( b < 0 )
    b = 0;
  // finally set Clyde's eye color.
  clyde.setEyeRGB( r, g, b );
}

// just start a color cycle and initialize the parameters (i.e. global variables)
// s_colors and s_durations have to be arrays (even if they have only length 1).
void startRGBCycle( uint8_t s_colors[], uint32_t s_durations[], uint8_t no_steps ){
  current_step = 1;
  // number_steps = sizeof( s_durations )/sizeof( s_durations[0] ); // doesn't work... unfortunately...
  number_steps = no_steps;
  step_start_r = clyde.current_colour[ 0 ]; // red value from which we start.
  step_start_g = clyde.current_colour[ 1 ]; // green value from which we start.
  step_start_b = clyde.current_colour[ 2 ]; // blue value from which we start.
  step_start = millis();
  // copy the content of the submitted arrays to the global variables
  for( int i=0; i < number_steps; i++ ){
    step_durations[ i ] = s_durations[ i ];
  }
  for( int i=0; i < number_steps * 3 ; i++ ){
    step_colors[ i ] = s_colors[ i ];
  }
#ifdef BASIC_DEBUG
  Serial << "Basic: startRGBCycle called" << " no steps: " << number_steps << endl;
#endif
}

// stop a color cycle. Will essentially just set the number of steps to 0
void stopCycle(){
  number_steps= 0;
  wl_number_steps=0;
  current_step = 0;
  wl_current_step = 0;
  cycle_rgb = false;
  cycle_wl = false;
}

// switches the light.
// if off -> switch on eye
// if eye on -> switch on white light
// if eye and white light on -> switch of eye
// if eye off and white light on -> switch off white light.
// Note: would like to get public access to the white light and to the threeway_max
void switchLights(){
  if( wl_intensity==0 ){
    // white light is off.
    if( clyde.current_colour[ 0 ] > 0 || clyde.current_colour[ 1 ] > 1 || clyde.current_colour[ 2 ] > 0 ){
      // got LED on, turn on white light.
      uint8_t newwl[1]={255};
      startFadeWhiteLight( newwl, fadetime, 1 );
            // reset the touchy feely. that's similar to the original Clyde firmware...
      //touchyfeely.reset( true, TOUCH_LEVEL*4, RELEASE_LEVEL*2);
    }else{
      // all is off, turn on led.
      startRGBCycle( COLORBREWER_BLUE, fadetime, 1 );
      //touchyfeely.reset( false, TOUCH_LEVEL, RELEASE_LEVEL);
    }
  }else{
    // white light is on.
    if( clyde.current_colour[ 0 ] > 0 || clyde.current_colour[ 1 ] > 1 || clyde.current_colour[ 2 ] > 0 ){
      // got LED on, turn off led.
      uint8_t lightoff [3] = {0, 0, 0};
      startRGBCycle( lightoff, fadetime, 1 );
    }else{
      // LED is off, turn off all lights
      uint8_t newwl[1]={0};
      //clyde.setLight( wl_intensity );
      startFadeWhiteLight( newwl, fadetime, 1 );
      //touchyfeely.reset( false, TOUCH_LEVEL, RELEASE_LEVEL);
    }
  }
#ifdef ENABLE_SPEAK
  dido();
#endif
  // calling reset on the sensor...again.
  //touchyfeely.reset( touchyfeely_auto, TOUCH_LEVEL, RELEASE_LEVEL );
}

// start a sunrise RGB cycle
void sunrise(){
  uint8_t sunrise_colors[ 21 ] = { 19, 17, 28, 39, 34, 57, 78, 69, 114, 46, 108, 181, 168, 142, 127, 255, 166, 48, 255, 210, 66 };
  uint32_t sunrise_durations[ 7 ] = {30000, 60000, 60000, 60000, 60000, 60000, 60000};
  startRGBCycle( sunrise_colors, sunrise_durations, 7 );
  // in addition, switch on the white light... slowly...
  uint8_t the_wl[ 1 ] = { 255 };
  uint32_t the_wl_dur[ 1 ] = { 460000 };
  startFadeWhiteLight( the_wl, the_wl_dur, 1 );
}

// start a sunset RGB cycle
void sunset(){
  uint8_t sunset_colors[ 27 ] = { 242, 103, 31, 201, 27, 38, 156, 15, 95, 96, 4, 122, 22, 10, 71, 12, 5, 35, 5, 0, 15, 0, 0, 5, 0, 0, 0 };
  uint32_t sunset_durations[ 9 ] = {30000, 30000, 40000, 60000, 60000, 60000, 60000, 60000, 60000};
  // start the RGB color cycle
  startRGBCycle( sunset_colors, sunset_durations, 9 );
}

/////////////////////////////////////////
//
//   color cycle stuff

// returns true if we're in a color cycle.
bool inCycle(){
  if( cycle_rgb || cycle_wl )
    return true;
  return false;
  //return ( cycle_rgb || cycle_wl );
}

// start either a color select or a brightness cycle, depending on the light status.
// if rgb on and wl not -> rgb color select.
// if wl on -> brightness select
// if both off -> switchLight.
void startCycle(){
  if( wl_intensity < 10 ){
    // do a rgb color cycle, if rgb is on.
    if( clyde.current_colour[ 0 ] > 0 || clyde.current_colour[ 1 ] > 0 || clyde.current_colour[ 2 ] > 0 ){
      cycle_rgb = true;
    }else{
      switchLights();
    }
  }else{
    // do a brightness cycle.
    cycle_wl = true;
  }
}

// updates a color cycle, if one is on.
void updateCycle(){
  uint32_t cycle_current_time = millis();
  if( ( cycle_current_time - cycle_last_update_ms ) < CYCLE_SLEEP_MS ){
    // don't update too often; otherwise the color change would be too fast.
    return;
  }
  cycle_last_update_ms = cycle_current_time;
  if( cycle_wl ){
    // cycle the white light.
    if( wl_intensity >= 245 ){
      wl_increasing = false;
    }else if( wl_intensity <= 10 ){
      wl_increasing = true;
    }
    if( wl_increasing ){
      wl_intensity+=4;
    }else{
      wl_intensity-=4;
    }
    clyde.setLight( wl_intensity );
  }else{
    if( cycle_rgb ){
      float cycle_hsv[3];
      rgbToHsv( clyde.current_colour[ 0 ], clyde.current_colour[ 1 ], clyde.current_colour[ 2 ], cycle_hsv );
      Serial << "what's the hsi? " << cycle_hsv[ 0 ] << " " << cycle_hsv[ 1 ] << " " << cycle_hsv[ 2 ] << endl;
      if( cycle_hsv[ 0 ] >= 0.95f ){
      cycle_hsv[ 0 ] = 0.0f;
      }
      cycle_hsv[ 0 ]+=0.02f;
      // convert back to rgb:
      uint8_t new_rgb[3];
      hsvToRgb( cycle_hsv[ 0 ], cycle_hsv[ 1 ], cycle_hsv[ 2 ], new_rgb );
      clyde.setEyeRGB( new_rgb[ 0 ], new_rgb[ 1 ], new_rgb[ 2 ] );
    }
  }
}


// Color converting; some modidied stuff from https://github.com/ratkins/RGBConverter
// mostly we ensure to use floats and stuff.
/**
* Converts an RGB color value to HSV. Conversion formula
* adapted from http://en.wikipedia.org/wiki/HSV_color_space.
* Assumes r, g, and b are contained in the set [0, 255] and
* returns h, s, and v in the set [0, 1].
*
* @param Number r The red color value
* @param Number g The green color value
* @param Number b The blue color value
* @return Array The HSV representation
*/
void rgbToHsv(uint8_t r, uint8_t g, uint8_t b, float hsv[]) {
  float rd = (float)r/255.0f;
  float gd = (float)g/255.0f;
  float bd = (float)b/255.0f;
  float max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
  float h, s, v = max;
  float d = max - min;
  s = max == 0 ? 0 : d / max;
  if (max == min) {
    h = 0; // achromatic
  } else {
    if (max == rd) {
      h = (gd - bd) / d + (gd < bd ? 6 : 0);
    } else if (max == gd) {
      h = (bd - rd) / d + 2;
    } else if (max == bd) {
      h = (rd - gd) / d + 4;
    }
    h /= 6;
  }
  hsv[0] = h;
  hsv[1] = s;
  hsv[2] = v;
}

/**
* Converts an HSV color value to RGB. Conversion formula
* adapted from http://en.wikipedia.org/wiki/HSV_color_space.
* Assumes h, s, and v are contained in the set [0, 1] and
* returns r, g, and b in the set [0, 255].
*
* @param Number h The hue
* @param Number s The saturation
* @param Number v The value
* @return Array The RGB representation
*/
void hsvToRgb(float h, float s, float v, uint8_t rgb[]) {
  float r, g, b;
  int i = int(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);
  switch(i % 6){
  case 0: r = v, g = t, b = p; break;
  case 1: r = q, g = v, b = p; break;
  case 2: r = p, g = v, b = t; break;
  case 3: r = p, g = q, b = v; break;
  case 4: r = t, g = p, b = v; break;
  case 5: r = v, g = p, b = q; break;
  }
  rgb[0] = (uint8_t)(r * 255);
  rgb[1] = (uint8_t)(g * 255);
  rgb[2] = (uint8_t)(b * 255);
}

float threeway_max(float a, float b, float c) {
  return max(a, max(b, c));
}
float threeway_min(float a, float b, float c) {
  return min(a, min(b, c));
}



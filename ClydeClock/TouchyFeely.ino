// note, cycleRGB is defined in Behaviour.ino
// Update: we require also a "double-touch" to avoid false positive triggers.
uint32_t last_touch_time[ 6 ] = {0};
uint8_t touch_counts[ 6 ] = {0};
uint16_t TOUCH_TRIGGER_DELAY = 500;  // trigger an action if the last

// multitouch:
// - use an array to record which legs have been touched
// - use a trigger delay after which an action will be triggered; depending on the legs
//   that have been touched. Also, reset the array.

void clydeReleasedOld(uint8_t l) {
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "Callback- released! " << l << ", leg switch would be " << leg_switch << endl;
#endif
  if( l==leg_switch ){
#ifdef TOUCHY_FEELY_DEBUG
    Serial << "got the switch leg" << endl;
#endif
    switchLights();
    //switchWL();
    return;
  }
  // only if the RGB light is on...
  if( clyde.current_colour[ 0 ] > 0 || clyde.current_colour[ 1 ] > 0 || clyde.current_colour[ 2 ] > 0 ){
    switch(l) {
    case 0:
      // cycle red
      startRGBCycle( COLORBREWER_RED, fadetime, 1 );
      break;
    case 1:
      // cycle green
      startRGBCycle( COLORBREWER_GREEN, fadetime, 1 );
      break;
    case 2:
      // cycle blue
      startRGBCycle( COLORBREWER_BLUE, fadetime, 1 );
      break;
    case 3:
      startRGBCycle( COLORBREWER_PURPLE, fadetime, 1 );
      break;
    case 4:
      startRGBCycle( COLORBREWER_ORANGE, fadetime, 1 );
      break;
    case 5:
      startRGBCycle( COLORBREWER_PINK, fadetime, 1 );
      break;
    }
  }
}


void clydeReleased( uint8_t l ){
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "Callback- released!" << l << endl;
#endif
  checkTouch( l );
}

void clydeTouched(uint8_t l) {
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "Callback- touched!" << l << endl;
#endif
}

void clydeDetected(uint8_t l) {
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "Callback- Detected!" << l << endl;
#endif
}

// problem with tha approach above is that it does not record the order of the touches.
/* // we're counting the number of touches within a certain time window. This can be */
/* // used later to trigger an action e.g. only if the same leg was touched twice to */
/* // avoid false positive touches. */
/* void checkTouch(uint8_t leg){ */
/*   uint32_t current_touch_time = millis(); */
/*   if( ( current_touch_time - last_touch_time[ leg ] ) < TOUCH_TRIGGER_DELAY ){ */
/*     // there was an additional touch within the time window... */
/*     touch_counts[ leg ] = touch_counts[ leg ] + 1; */
/*   }else{ */
/*     touch_counts[ leg ] = 1; */
/*   } */
/*   last_touch_time[ leg ] = current_touch_time; */
/* } */

/* // loop through the whole array and trigger some action... */
/* void evaluateTouchArray(){ */
/*   // loop through the array and get the most recent touch time. If that was more than */
/*   // TOUCH_TRIGGER_DELAY do something (and remember to reset all!). */
/*   uint32_t most_recent_touch = 0; */
/*   for( int i=0; i < 6; i++ ){ */
/*     if( last_touch_time[ i ] > most_recent_touch ){ */
/*       most_recent_touch = last_touch_time[ i ]; */
/*     } */
/*   } */
/*   // so, if the last touch is at least TOUCH_TRIGGER_DELAY ms ago, go through */
/*   // the full array... */
/*   if() */
/* } */


// alternative approach: push the leg number and the time of touch to an
// array. if the most recent touch (always 0) is more than TOUCH_TRIGGER_DELAY ago,
// evaluate the full array, but require always double touches (to avoid false positives).
uint32_t touch_array_last_touch_time = 0;
uint8_t touch_array_touched_leg[ 8 ] = {31};
boolean require double_touch = true;
//char touch_string[8];

// inserts the leg-number at position 0 in the array and shifts all previous numbers.
void addToTouchArray( uint8_t leg ){
  // shift all values in the array
  for( uint8_t current_pos < 7; current_pos >= 0; current_pos-- ){
    touch_array_touched_leg[ current_pos + 1 ] = touch_array_touched_leg[ current_pos ];
  }
  // add the leg number to the stack and record the last touch time.
  touch_array_touched_leg[ 0 ] = leg;
  touch_array_last_touch_time = millis();
}

void evalTouchTimeArray(){
  // get the most recent entry in touch_array_times
  if( ( millis() - touch_array_times[ 0 ] ) > TOUCH_TRIGGER_DELAY ){
    // evaluate the stack.
    // go through the array and report all duplets (i.e. double touches of the same leg)
    uint8_t lastleg=31;
    char touch_string[4]; // should be touch_array_touched_leg/2
    for( int i=0; i < 8; i++ ){
      if( lastleg!=touch_array_touched_leg[ i ] ){
	lastleg = touch_array_touched_leg[ i ];
      }else{
	if( lastleg!=31 ){
	  // report the lastleg, maybe add to character string?
	  //int aInt = 368;
	  //char str[15];
	  //sprintf(str, "%d", aInt);
	  //strcat( touch_string, lastleg );
	  char num_str[1];
	  sprintf( num_str, "%d", lastleg );
	  strcat( touch_string, num_str );
	}
	lastleg = 31;  // set that to 31, so we're looking for a new duplet.
      }
    }
    if( strcmp( touch_string, "1" )==0 ){
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "evalTouchArray! " << l << "got 1: " << touch_string << endl;
#endif
    }else if( strcmp( touch_string, "2" )==0 ){
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "evalTouchArray! " << l << "got 2: " << touch_string << endl;
#endif
    }else{
#ifdef TOUCHY_FEELY_DEBUG
  Serial << "evalTouchArray! " << l << "got something else: " << touch_string << endl;
#endif
    }
    // check: if touch_string not empty:
    // compare strings: strcmp( stringa, stringb), returns 0 if they are equal.
    // make a switch thing
    // reset the stack!
  }
}

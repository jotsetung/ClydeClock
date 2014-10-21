<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. Clyde Clock</a>
<ul>
<li><a href="#sec-1-1">1.1. Requirements</a></li>
<li><a href="#sec-1-2">1.2. Configurations</a></li>
<li><a href="#sec-1-3">1.3. Functionality</a></li>
<li><a href="#sec-1-4">1.4. Files and functions</a></li>
</ul>
</li>
</ul>
</div>
</div>



---

# Clyde Clock<a id="sec-1"></a>

## Requirements<a id="sec-1-1"></a>

-   The great `ClydeDev` library from RobotGrrl (<https://github.com/RobotGrrl/ClydeDev.git>).
-   The `Streaming` (<http://arduiniana.org/libraries/streaming/>) library.
-   The `Time` (<https://github.com/PaulStoffregen/Time.git>) library.
-   The modified `TimeAlarms` (<https://github.com/jotsetung/TimeAlarms.git>) library (allows to call function `serviceAlarms` directly).
-   Remove the original `Clyde` library from the Arduino libraries folder.
-   Add all the libraries above to the Arduino libraries.

## Configurations<a id="Configurations"></a><a id="sec-1-2"></a>

-   **!** Adapt `ClydeTouchyFeely(1)` and `ClydeAfraidDark(2)` in *ClydeClock.ino* (lines 31 and 35, respectively) depending on where you installed the *TouchyFeely* and the *Afraid of the dark* modules.

-   Use one of the legs instead of the *eye* to switch the lights on or off: setting the `leg_switch` variable to a value from 0-5 will cause the respective leg to be used as the *switch*. This is useful for Clydes with a buggy IR sensor in the eye.

-   Define custom sunset or sunrise colors and duration in *Basic.ino*'s `sunrise` and `sunset` functions.

## Functionality<a id="Functionality"></a><a id="sec-1-3"></a>

-   Change the RGB light to a different color for each of Clyde's legs.

-   Use one of the legs as the light switch (instead of the eye; see 1.2 section above).

## Files and functions<a id="sec-1-4"></a>

-   *Afraid.ino*: process readouts from the light sensor (AfraidOfTheDark).
    -   `checkForDarkness`: compares current and previous light intensities, and if they are below a certain threshold for a specified time it triggers the `sunset`.

-   *Basic.ino*: provides basic functions.
    -   `void cycleThroughRGBColors()`: cycles through the colors defined by the global variable `step_colors` for the pre-defined durations (global variable `step_durations`). This function is intended to be called in the `update` function and requires global parameters to be set (e.g. *via* `startRGBCycle`).
    
    -   `void fadeWhiteLight()`: similar to the function above, allows to cycle the white light intensities through a pre-defined set of intensities (for pre-defined durations). This function is called by the `update` function, the intensity steps and durations can be defined with the function `startFadeWhiteLight`.
    
    -   `void startRGBCycle( uint8_t s_colors[], uint32_t s_durations[], uint8_t no_steps )`: sets the colors and durations for the color cycle. `s_colors` should be an array with r,g,b values from 0 to 255, `s_durations` an array with the time (in ms) to reach the defined color (length of the array should thus be length of `*s_durations` / 3); `no_steps` should be the length of the `s_durations` array.
    
    -   `void startFadeWhiteLight( uint8_t intens[], uint32_t durations[], uint8_t no_steps )`: same as above, just for the white light. Also, since only a single light intensity is defined instead of r, g and b values, the length of `intens` and `durations` have to match and have to be equal to `no_steps`.
    
    -   `void stopCycle()`: just resets the cycle step for rgb and white light and thus exits any cycle.
    
    -   `void sunrise()`: defines sunrise colors and starts an RGB cycle through these.
    
    -   `void sunset()`: defines sunset colors and starts an RGB cycle through these.
    
    -   `void switchLights()`: function to switch the white light and rgb light on and off.

-   *ClydeClock.ino*: the main file.

-   *Eye.ino*: functions related to the eye (handler functions).

-   *Mike.ino*: functions related to a connected microphone. This can be enables/disabled with the `#define ENABLE_MIKE` in *ClydeClock.ino*.

-   *Time.ino*: functions related to time and alarm clock. This can be enables/disabled with the `#define ENABLE_CLOCK` in *ClydeClock.ino*.

-   *TouchyFeely.ino*: contains the handler function to be called when one of Clyde's legs is touched.
    -   `void clydeTouched(uint8_t l)`: the handler function called if a leg of Clyde is touched. The function will change the rgb light to a pre-defined color depending on which leg was touched. Also, if `leg_switch` is set to a value from 0 to 5, it calls the `switchLights` function (useful for those Clydes for which the eye does no longer work&#x2026;).
    
    -   `void clydeReleased(uint8_t l)`: the handler function called if a leg of Clyde is touched.
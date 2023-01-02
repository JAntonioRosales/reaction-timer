
# Programming Embedded Systems Assignment

## Project description

The objective of this assignment is to implement a reaction test timer using the ARM development board.

The reaction timer displays the time it takes for the user to react to an LED being illuminated. It displays the time in seconds to an accuracy of one thousandth of a
second.

* 1. Upon start-up, or after a reaction test has been completed, LED 1 should flash to indicate Button 1 is to be pressed to start the next reaction test.
* 2. When Button 1 is pressed the display should be zeroed, all the LEDs are turned RED and begin to count down (turn off) at the rate of one per second. The count down
can be from left to right or right to left.
* 3. When the last LED turns off a random LED is lit (turned GREEN) indicating the button that should be pressed to stop the count on the display. At the same time
the display begins counting, showing the elapsed time.
* 4. When the indicated button is pressed the display stops counting. The timer is ready for the next reaction test (returns to step 1).

If the user presses any button other than the one indicated by the green LED the count should continue. If the user fails to react, the count stops at 9.999 seconds.

If the user presses one or more buttons before the countdown of LEDs finishes, the countdown is suspended until all the buttons are released.

## Code structure

The code developed for the project is named main.c and it can be found in this same directory.

The first part of the code declares a set of variables used throughout the program. Afterwards, a few routines are defined to be used throughout the code. Finally, the three main parts of the implementation are developed: initialization (flashing), countdown, and reaction.

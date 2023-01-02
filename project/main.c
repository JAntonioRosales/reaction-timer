/*
 * CE865 Assignment - Jose Antonio Rosales Medina ROSAL89806
 */

/* Inclusion of different libraries that provide access to specific functions */
#include <config/AT91SAM7S256.h> /* Hardware description header */
#include <board/Button-lib.h> /* Provides access to the buttons */
#include <board/LED-lib.h> /* Provides access to the LEDs */
#include <board/SevenSegmentDisplay.h> /* Provides access to the 7 segment display functions */
#include <stdlib.h> /* Provides access to the general purpose standard library */
#include <atmel/pit.h> /* Provides access to the Periodic Interval Timer (PIT) */
#include <atmel/aic.h> /* Provide access to the Advanced Interrupt Controller (AIC) */

/*
 * Name definition for the 4 displays of the 7 Segment Display. These make up the digits of the timer
 * and the names make the code more understandable.
 * (The rightmost digit is DISPLAY1 and the leftmost digit is DISPLAY4)
 */
#define M_SECONDS			DISPLAY1
#define H_SECONDS			DISPLAY2
#define T_SECONDS			DISPLAY3
#define SECONDS				DISPLAY4

/*
 * The following arrays contain the binary signals (in hexadecimal representation) used to display
 * the decimal digits 0-9 in the 7 Segment Display.
 *
 * The secDigit array has been modified to include the decimal point when displaying the number.
 *
 * These binary signals directly map to each LED of each 7 Segment Display.
 * (The SevenSegmentDisplay library section in the manual contains more information about this)
 */
short Digit[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
short secDigit[] = { 0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef };

/* Definition of global variables */
LEDnumber LEDoff; /* Stores the number of the LED that is turned off during the countdown */
LEDcolour lastLEDcolor; /* Stores the color of the last LED in the countdown */
short m_seconds, h_seconds, t_seconds, seconds, hertz; /* Variables for fractions of a second and the interrupt frequency */
short rnum; /* Stores a random number to randomly turn on a green LED */
short countUp; /* Variable for the timer's condition to count or stop counting */

/* PIT Interrupt period definition */
#define PIT_PERIOD          1000 /* Timer interrupt every millisecond [1000us] (freq. = hertz = 1/0.001s = 1000Hz = 1kHz) */





//--------------------------- DEFINED FUNCTIONS ------------------------------------------------------------------------------





/*
 * DELAY SOFTWARE ROUTINE
 * Waits a number of approximate milliseconds, as seen in the "Laboratory 1" code.
 */
void delay_ms (short ms)
{
	volatile short loop;
	while (ms-- > 0)
	{
		for (loop = 0; loop < 2100; loop++);
	}
}

/*
 * INTERRUPT ROUTINE
 * Called for every PIT interrupt (1000 interrupts in 1 second), modified from the "Laboratory 5" code.
 */
void ISR_System(void)
{
    PIT_GetPIVR(); /* Reads the PIT status & clears pending interrupts (not assigned - we're not interested in the result) */

    /*
     * Interrupt processing... (The previously defined period makes 1000 interrupts = 1 second)
     */

    if (++m_seconds == 10) /* Increases the millisecond count and sets the tenth millisecond back to 0 */
        	m_seconds = 0;

    if ((hertz % 10) == 0) /* Identifies a hundredth of a second */
	{
		if (++h_seconds == 10) /* Increases the hundredth count and sets the tenth hundredth of a second back to 0 */
			h_seconds = 0;
	}

    if ((hertz % 100) == 0) /* Identifies a tenth of a second */
    {
    	if (++t_seconds == 10) /* Increases the tenth count and sets the tenth tenth of a second back to 0 */
    		t_seconds = 0;
    }

    if (++hertz == 1000) /* Increases the interrupt (hertz) count and sets the thousandth interrupt back to 0 */
    {
    	hertz = 0;

		if (++seconds == 10) /* Increases the second count and sets the tenth second back to 0 */
			seconds = 0;
    }

}





//----------------------------- PROGRAM FUNCTIONS ----------------------------------------------------------------------------





/* Program execution begins here! */

/*
 * Initialization and program start
 */
int main()
{
	Configure7SegmentDisplay(); /* Initializes the 7 Segment Display */

	flash_LED(); /* Calls the flash_LED() function */

	return 0;
}

/*
 * LEDs reset and blinking of LED1
 */
void flash_LED()
{
	/* Displays the last recorded timer values on the 7 Segment Display (shows 0.000 the first time it runs) */
	Set7SegmentDisplayValue(M_SECONDS, Digit[m_seconds]);
	Set7SegmentDisplayValue(H_SECONDS, Digit[h_seconds]);
	Set7SegmentDisplayValue(T_SECONDS, Digit[t_seconds]);
	Set7SegmentDisplayValue(SECONDS, secDigit[seconds]);

	/* Turns all the LEDs off */
	SetAllLEDs(LEDsAllOff);

	/* Prevents auto-initialization when the random green LED in reaction() was LED1 */
	delay_ms(250);

	/* This loop blinks LED1 red until Button 1 is pressed */
	while(IsButtonReleased(BUTTON1))
	{
		SetLEDcolor(LED1, RED);
		delay_ms(250);
		if(IsButtonPressed(BUTTON1)) break; /* Lets us exit the while loop before its execution finishes */
		SetLEDcolor(LED1, OFF);
		delay_ms(250);
	}

	countdown(); /* Calls the countdown() function */
}

/*
 * LED countdown for the reaction test
 */
void countdown()
{
	/* Resets the 7 Segment Display and shows 0 for each digit */
	Set7SegmentDisplayValue(M_SECONDS, Digit[0]);
	Set7SegmentDisplayValue(H_SECONDS, Digit[0]);
	Set7SegmentDisplayValue(T_SECONDS, Digit[0]);
	Set7SegmentDisplayValue(SECONDS, secDigit[0]);

	/* Sets all the LEDs RED */
	SetAllLEDs(LEDsAllRed);
	delay_ms(1000);

	/* Determines the first LED to turn off and the last LED to continue with the reaction test once it is turned off */
	LEDoff = LED1;
	lastLEDcolor = GetLEDcolor(LED8);

	/* This loop runs until the last LED turns off */
	while (lastLEDcolor != 0)
	{
		/* The countdown begins when no button is pressed */
		if (IsButtonReleased(BUTTON1) && IsButtonReleased(BUTTON2) && IsButtonReleased(BUTTON3) && IsButtonReleased(BUTTON4) && IsButtonReleased(BUTTON5) && IsButtonReleased(BUTTON6) && IsButtonReleased(BUTTON7) && IsButtonReleased(BUTTON8))
		{
			/* LED countdown */
			for (short i = 0; i < 8; i++)
			{
				SetLEDcolor(LEDoff, OFF);
				LEDoff++; // Prepares the next LED to be turned off
				lastLEDcolor = GetLEDcolor(LED8); // Checks fos the last LED's status
				delay_ms(1000); // One LED is turned off every second
				/* If any button is pressed, the countdown will stop until the button(s) are released*/
				while (IsButtonPressed(BUTTON1)||IsButtonPressed(BUTTON2)||IsButtonPressed(BUTTON3)||IsButtonPressed(BUTTON4)||IsButtonPressed(BUTTON5)||IsButtonPressed(BUTTON6)||IsButtonPressed(BUTTON7)||IsButtonPressed(BUTTON8)){}
			}
		}
	}

	reaction(); /* Calls the reaction() function */
}

/*
 * Reaction test
 */
int reaction()
{
	/*
	 * Randomly generate a number between 0 and 7 to turn on a green LED
	 *
	 * Instruction retrieved from "C library function - rand() by tutorialspoint"
	 * https://www.tutorialspoint.com/c_standard_library/c_function_rand.htm
	 */
	rnum = rand() % 7;
	SetLEDcolor(rnum, GREEN);

	/* Resets the time variables to 0 */
	hertz = 0;
	seconds = 0;
	t_seconds = 0;
	h_seconds = 0;
	m_seconds = 0;

	/* Displays the time variables' values in the 7 Segment Display */
	Set7SegmentDisplayValue(M_SECONDS, Digit[m_seconds]);
	Set7SegmentDisplayValue(H_SECONDS, Digit[h_seconds]);
	Set7SegmentDisplayValue(T_SECONDS, Digit[t_seconds]);
	Set7SegmentDisplayValue(SECONDS, secDigit[seconds]);

	/* Initializes and enables the PIT */
	PIT_Init(PIT_PERIOD, BOARD_MCK / 1000000); /* (millisec. between timer interrupts, clock rate in MHz - in Hz so divide by 1E6) */

	AIC_DisableIT(AT91C_ID_SYS); /* Disables the interrupt on the interrupt controller */

	/*
	 * Configures the AIC for PIT interrupts (which is a system interrupt with its own ID)
	 */
	AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System); /* Associates the PIT's ID with the ISR_System interrupt routine */
	AIC_EnableIT(AT91C_ID_SYS); /* Enables the interrupt on the AIC */
    PIT_EnableIT(); /* Enables the PIT interrupt - done first so no interrupts are missed once the timer is started */
    PIT_Enable(); /* Starts the PIT  */

    countUp = 1; /* Variable for the timer */

    /* This loop will run until the countUp variable is different than 1 */
    while(countUp == 1)
    {
    	/* Displays the time variables' values in the 7 Segment Display (variables updated in the ISR_System interrupt routine) */
    	Set7SegmentDisplayValue(M_SECONDS, Digit[m_seconds]);
		Set7SegmentDisplayValue(H_SECONDS, Digit[h_seconds]);
		Set7SegmentDisplayValue(T_SECONDS, Digit[t_seconds]);
		Set7SegmentDisplayValue(SECONDS, secDigit[seconds]);

		/* Conditions to stop the timer: if the button of the GREEN LED is pressed or if the timer gets to 9.999 seconds */
		if (IsButtonPressed(rnum)) countUp = 0;
		else if ((m_seconds == 9) && (h_seconds == 9) && (t_seconds == 9) && (seconds == 9)){
			countUp = 0;
		}
    }

    flash_LED(); /* Calls the flash_LED() function to restart the test all over again */

    return 0;
}

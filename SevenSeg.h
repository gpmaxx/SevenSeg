/* 	Greg Lipscomb - June 29, 2017

	June 29, 2017  - 	Inital Version
	August 3, 2017 - 	Clean up and additional documentation

	Basic library for controlling a 7 segment (plus decimal) display with  a shift register

	ToDo:   Display of float functionality
				- very messy/difficult to implement because Arduino's implementation of sprint doesn't support floats
			Has only really been tested with a 4 digit display. Needs to be tested with more digits.
*/

#ifndef SEVENSEG
#define SEVENSEG

#include "Arduino.h"

class SevenSeg
{
	public:

		// dataPin - the arduino pin connected to the data of the shift register
		// latchPin - the arduino pin connected to the latch of the shift register
		// clockPin - the arduino pin connected to the clock of the shift register
		// numDigits - the number of digits in the display
		// digitPins - array (numDigits long) of pins which control the each of the display digits
		// segmentOnHigh - true if to turn on a segment it should be set high
		// digitOnHigh - true if to turn on a digit it should be set high
		SevenSeg(const uint8_t dataPin, const uint8_t latchPin, const uint8_t clockPin, const uint8_t numDigits, uint8_t *digitPins, const bool segmentOnHigh, const bool digitOnHigh);

		// must be called in a loop for the display to multiplex and operate properly
		void updateDisplay();

		// set the display's value to a particular number
		void setDisplayNum(const int16_t theNumber);

		// set the display's value to a particular number and turn on the decimal point at the provided index
		void setDisplayNum(const int16_t theNumber, const uint8_t decimalPosition);

		// directly set the display's value.  This function is useful when setting "codes" or "pins" which are
		// not nessisarily ints.  E.G. To set the display value to 0012 this function can be used.  The chars
		// in the provided string must be valid characters for the display i.e. '0' -> '9' or '-'
		void setDisplayText(const char* theValue);

		// directly set the display's value and turn on the decimal at the provided index
		void setDisplayText(const char* theValue, const uint8_t decimalPosition);

		// directly set the segments using a bitmask
		// values is array with 1 element for each digit of the display
		// see the bitmask section below for further details
		void setBitmapValue(const uint8_t values[]);

		// sets the right most digit of the display (and it's decimal point)
		// shifts all other values left
		void setDisplayDigitAndShift(const uint8_t theDigit, const bool decimalOn);

		// sets the right most digit of the display and shifts all other values left
		void setDisplayDigitAndShift(const uint8_t theDigit);

		// set the right most digit of the display and shifts all other values left
		void setDisplayBitmapAndShift(const uint8_t bitmap);

		// returns the display's current display number
		// this value may not be valid/current depending on how the display was last set
		int16_t getDisplayNum();

		// turns the display to error mode
		void setErrorDisplay();

		// demo function which displays circles
		// totalTime - time in millis to run
		// segmenttime - time in millis to pause for each segment
		// clockWise - circle clockwise if true
		// small - small circle mode if true
		void circles(const uint32_t totalTime, const uint16_t segmenttime, const bool clockWise, const bool small);

		void on();
		void off();
		bool isOn();  // public version of getDisplayIsOn

	private:
		// display the char at specified digit index.  Decimal point on if decimalOn is true
		void displayDigit(const char theDigit, const uint8_t pos, const bool decimalOn);

		// display the provided bitmap at the specified digit ind\ex
		void displayBitmap(const uint8_t bitmap, const uint8_t pos);

		// all segments off
		void segmentsOff();

		// turn digit on at index
		void digitOn(const uint8_t pinsArrayIndex);

		// turn digit off at index
		void digitOff(const uint8_t pinsArrayIndex);

		// all digits off
		void digitsOff();

		// set the segment configuration to the provided bitmap
		void setSegments(const uint8_t bitMap);

		// validate & format theNumber for display setting the displayText value
		void convertDisplayNum(const int16_t theNumber);

		// 10 to the power of exp calculated
		uint16_t tenToThePower(const uint8_t exp);

		// moves display text to the right most digit
		// i.e number value of 1 would display as 1 _ _ _
		// this shifts to _ _ _ 1
		uint8_t shiftDisplayText();

		// demo function small circles
		void circlesSmall(const uint32_t totalTime, const uint16_t segmentTime, const bool clockWise);

		// demo function big circles
		void circlesBig(const uint32_t totalTime, const uint16_t segmentTime, const bool clockWise);


		// Getters and Setters
		uint8_t getDisplayNumDigits();
		uint8_t getDataPin();
		uint8_t getLatchPin();
		uint8_t getClockPin();

		char* getDisplayText();
		void clearDisplayText();
		uint8_t getDigitPin(const uint8_t index);
		bool getDisplayIsOn();
		void setDisplayIsOn(const bool isOn);
		uint8_t getDecimalPosition();
		void setDecimalPosition(const uint8_t decimalPosition);
		uint16_t mod(const int16_t a, const int16_t b);  // need a true modulus function as % gives remainder we need always positive value
		bool getRawMode();
		void setRawMode(const bool isRawMode);

		// variables
		bool _displayIsOn = false;
		uint8_t _displayNumDigits;
		uint8_t _dataPin;
		uint8_t _latchPin;
		uint8_t _clockPin;
		uint8_t *_digitPins;
		int16_t _displayNumber = 0;
		char *_displayText;
		uint8_t _decimalPosition = 0;
		bool _rawMode = false;

		const char EMPTY_MARKER = 'x';
		const char NEGATIVE_SIGN = '-';
		const bool SEGMENT_ON_FLAG;
		const bool SEGMENT_OFF_FLAG;
		const bool DIGIT_ON_FLAG;
		const bool DIGIT_OFF_FLAG;
		const uint8_t MULTIPLEX_DELAY = 2;

		/* Bitmask values */

		// bitmap segments values are set as:
		/*   777
		    2   6
		    2   6
		     111
		    3   5
		    3   5
		     444   0

		Where 7 (top segment) is most significant bit and 0 (decimal point) is least significant
		*/
		const uint8_t SEGMENTS_OFF_LAYOUT = B00000000;
		const uint8_t DIGIT_LAYOUT[10] = {B11111100,B01100000,B11011010,B11110010,B01100110,B10110110,B00111110,B11100000,B11111110,B11100110};
		const uint8_t NEGATIVE_LAYOUT = B00000010;
};


#endif
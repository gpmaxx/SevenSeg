#include "SevenSeg.h"

SevenSeg::SevenSeg(const uint8_t dataPin, const uint8_t latchPin, const uint8_t clockPin, const uint8_t numDigits, uint8_t *digitPins, const bool segmentOnFlag, const bool digitOnFlag):
	_dataPin(dataPin),_latchPin(latchPin),_clockPin(clockPin),_displayNumDigits(numDigits),
	SEGMENT_ON_FLAG(segmentOnFlag),SEGMENT_OFF_FLAG(!segmentOnFlag),DIGIT_ON_FLAG(digitOnFlag),DIGIT_OFF_FLAG(!digitOnFlag)
{
	// digit pins and displayText array size is determined by the number of display digits
	_digitPins = malloc(sizeof(uint8_t)*getDisplayNumDigits());
	//_displayText = (char*)malloc(sizeof(char)*(getDisplayNumDigits()+1));  // one  extra for the end of string marker
	_displayText = (char*)malloc(5);
	clearDisplayText();
	
	for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
	{
		_digitPins[i] = *(digitPins+i);
		pinMode(getDigitPin(i),OUTPUT);
		digitalWrite(getDigitPin(i),DIGIT_OFF_FLAG);
	}

	pinMode(getDataPin(),OUTPUT);
	pinMode(getLatchPin(),OUTPUT);
	pinMode(getClockPin(),OUTPUT);

}

void SevenSeg::setDisplayNum(const int16_t theNumber)
{
	setDisplayNum(theNumber,-1);
}

void SevenSeg::setDisplayNum(const int16_t theNumber, const uint8_t decimalPosition)
{
	_displayNumber = theNumber;
	setDecimalPosition(decimalPosition);
	convertDisplayNum(getDisplayNum());
	setRawMode(false);
}

void SevenSeg::clearDisplayText()
{
	memset(_displayText,EMPTY_MARKER,getDisplayNumDigits());
	_displayText[getDisplayNumDigits()] = '\0';
}

void SevenSeg::setDisplayDigitAndShift(const uint8_t theDigit){ setDisplayDigitAndShift(theDigit,false);}
void SevenSeg::setDisplayDigitAndShift(const uint8_t theDigit, const bool decimalOn)
{
	if (getRawMode()) { return; }  // only accept new digits if we are in raw mode
	for (uint8_t i = 0; i < getDisplayNumDigits() - 1; i++)
	{
		_displayText[i] = _displayText[i+1];  // move every digit over 1 to the left
	}
	_displayText[getDisplayNumDigits()-1] = theDigit + '0';  // set right most value
	
	if (decimalOn)
	{
		setDecimalPosition(getDisplayNumDigits());  // override decimal position and set to right most
	}
	else if (getDecimalPosition() > 0)
	{
		setDecimalPosition(getDecimalPosition()-1);  // shift decimal
	}
}

void SevenSeg::setDisplayBitmapAndShift(const uint8_t bitmap)
{
	if (!getRawMode()) { return; }
	for (uint8_t i = 0; i < getDisplayNumDigits() - 1; i++)
	{
		_displayText[i] = _displayText[i+1];   // shift everything left 1
	}
	_displayText[getDisplayNumDigits()-1] = bitmap;  // set rightmost to the provided bitmask
}

void SevenSeg::setDisplayText(const char* theValue)
{
	setDisplayText(theValue,0);
}

void SevenSeg::setDisplayText(const char* theValue, const uint8_t decimalPosition)
{
	clearDisplayText();
	setDecimalPosition(decimalPosition);
	strncpy(_displayText,theValue,getDisplayNumDigits());
	setRawMode(false);
}

void SevenSeg::setBitmapValue(const uint8_t values[])
{
	clearDisplayText();
	for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
	{
		_displayText[i] = values[i];
	}
	setRawMode(true);
}

void SevenSeg::convertDisplayNum(const int16_t theNumber)
{
	// is number positive and small enough to fit on display?
	if ((theNumber >= 0) && (theNumber < tenToThePower(getDisplayNumDigits())))
  	{
    	itoa(theNumber,_displayText,10);
    	shiftDisplayText();
  	}
  	// is number negative and small enough to fit on the display?
  	else if ((theNumber < 0) && (abs(theNumber) < tenToThePower(getDisplayNumDigits() - 1)))
  	{
    	itoa(abs(theNumber),_displayText,10);
    	uint8_t shift = shiftDisplayText();
    	_displayText[shift-1] = NEGATIVE_SIGN;
  	}
  	else
  	{
    	setErrorDisplay();
  	}
}

// custom power function  because the pow function requires using doubles
uint16_t SevenSeg::tenToThePower(uint8_t exp)
{
  uint16_t result = 1;
  for (uint8_t i = 0; i < exp; i++)
  {
    result *= 10;
  }

  return result;
}

void SevenSeg::setErrorDisplay()
{
  for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
  {
    _displayText[i] = NEGATIVE_SIGN;
  }
}

uint8_t SevenSeg::shiftDisplayText()
{
	uint8_t shift = getDisplayNumDigits() - strlen(getDisplayText());

  	for (uint8_t i = getDisplayNumDigits()-1; i > 0;i--)
  	{
    	_displayText[i] = _displayText[i - shift];
  	}
  	for (uint8_t i = 0; i < shift;i++)
  	{
    	_displayText[i] = EMPTY_MARKER;
  	}

	if (getDecimalPosition() >= 0)
	{
		setDecimalPosition(getDecimalPosition()+1); 		// setDecimalPostion will reset value if this puts it out of bounds
	}

  	return shift;
}

void SevenSeg::displayDigit(const char theDigit, const uint8_t pos, const bool decimalOn)
{
  uint8_t bitMap;
  if ((theDigit >= '0') && (theDigit <= '9'))
  {
    bitMap = DIGIT_LAYOUT[theDigit-'0'];
    if (decimalOn) {bitMap |= 1;}   // set the low bit to turn on dp segment
  }
  else if (theDigit == NEGATIVE_SIGN)
  {
    bitMap = NEGATIVE_LAYOUT;
  }
  else
  {
    bitMap = SEGMENTS_OFF_LAYOUT;
  }

	displayBitmap(bitMap,pos);

}

void SevenSeg::displayBitmap(const uint8_t bitmap, const uint8_t pos)
{
	digitsOff();
	setSegments(bitmap);
	digitOn(pos);
}


void SevenSeg::setSegments(const uint8_t bitMap)
{
  digitalWrite(getLatchPin(),LOW);
  shiftOut(getDataPin(),getClockPin(),LSBFIRST,bitMap);
  digitalWrite(getLatchPin(),HIGH);
}

void SevenSeg::segmentsOff()
{
  setSegments(SEGMENTS_OFF_LAYOUT);
}

void SevenSeg::digitOn(const uint8_t pinsArrayIndex)
{
  digitsOff();
  digitalWrite(getDigitPin(pinsArrayIndex),DIGIT_ON_FLAG);
}

void SevenSeg::digitOff(const uint8_t pinsArrayIndex)
{
  digitalWrite(getDigitPin(pinsArrayIndex),DIGIT_OFF_FLAG);
}

void SevenSeg::digitsOff()
{
  for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
  {
    digitOff(i);
  }
}

void SevenSeg::on()
{
	setDisplayIsOn(true);
	updateDisplay();
}

void SevenSeg::off()
{
	for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
	{
		displayDigit(EMPTY_MARKER,i,false);
	}
	setDisplayIsOn(false);
}

void SevenSeg::circles(const uint32_t totalTime, const uint16_t segmentTime, const bool clockWise, const bool small)
{
	if (small)
	{
		circlesSmall(totalTime,segmentTime,clockWise);
	}
	else
	{
		circlesBig(totalTime,segmentTime,clockWise);
	}
}

void SevenSeg::circlesBig(const uint32_t totalTime, const uint16_t segmentTime, const bool clockWise)
{
	uint8_t index = 0;
	uint8_t bitMask = 0;
	uint32_t timer1 = millis();
	int8_t increment = 1;
	uint8_t segment = 0;
	uint8_t digit = 0;
	if (!clockWise) { increment = -1; }

	while ((millis() - timer1) < totalTime)
	{
		if (index < getDisplayNumDigits())
		{
			digit = index;
			segment = 0;
		}
		else if (index == getDisplayNumDigits())
		{
			digit = getDisplayNumDigits() - 1;
			segment = 1;
		}
		else if (index == getDisplayNumDigits() + 1)
		{
			digit = getDisplayNumDigits() - 1;
			segment = 2;
		}
		else if ((index > getDisplayNumDigits() + 1) && (index < ((getDisplayNumDigits() * 2) + 2)))
		{
			digit = ((getDisplayNumDigits() * 2) + 1) - index;
			segment = 3;
		}
		else if (index == ((getDisplayNumDigits() * 2) + 2))
		{
			digit = 0;
			segment = 4;
		}
		else
		{
			digit = 0;
			segment = 5;
		}
		bitMask = 1 << (7 - segment);
		setSegments(bitMask);
		digitOn(digit);
		delay(segmentTime); // there will only be 1 segment on at a time for the whole display so we don't need to multiplex
		index = mod(index + increment,(getDisplayNumDigits() * 2) + 4);
	}
	digitsOff();
}

void SevenSeg::circlesSmall(const uint32_t totalTime, const uint16_t segmentTime, const bool clockWise)
{
	uint8_t bitMask = 0;
	int8_t index = 0;
	uint32_t timer1 = millis();
	uint32_t timer2 = millis();
	int8_t increment = 1;
	if (!clockWise) {increment = -1;}

	const uint8_t segmentPattern[] = {2,3,4,5,6,1,2,7,6,1,2,3,4,5,7,3,4,5,6,1};

	while ((millis() - timer1) < totalTime)
	{
		if ((millis() - timer2) > segmentTime)
		{
			bitMask = (1 << (8 - segmentPattern[index]));
			setSegments(bitMask);
			timer2 = millis();
			index = mod(index + increment,sizeof(segmentPattern));
		}

		for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
		{
			digitOn(i);
			delay(MULTIPLEX_DELAY);
		}
	}
	digitsOff();

}

// turn modulo function to use instead of % which is really the remainder and not congrunace modulo
uint16_t SevenSeg::mod(const int16_t a, const int16_t b)
{
	int16_t r = a % b;
	return r < 0 ? r + b : r;
}

void SevenSeg::updateDisplay()
{

	if (getDisplayIsOn())
	{
		bool rawMode = getRawMode();
		for (uint8_t i = 0; i < getDisplayNumDigits(); i++)
		{
			if (rawMode)
			{
				displayBitmap(getDisplayText()[i],i);
			}
			else
			{
				displayDigit(getDisplayText()[i],i,i==(getDecimalPosition()-1));
			}

			delay(MULTIPLEX_DELAY);
		}
	}
}


// decimal position is indexed from 1 instead of 0
void SevenSeg::setDecimalPosition(const uint8_t decimalPosition)
{
	if ((decimalPosition > 0) && (decimalPosition <= getDisplayNumDigits()))
	{
		_decimalPosition = decimalPosition;
	}
	else
	{
		_decimalPosition = 0;
	}
}

/* getters and setters */
uint8_t SevenSeg::getDisplayNumDigits(){return _displayNumDigits;}
uint8_t SevenSeg::getDataPin(){return _dataPin;}
uint8_t SevenSeg::getLatchPin(){return _latchPin;}
uint8_t SevenSeg::getClockPin(){return _clockPin;}
int16_t SevenSeg::getDisplayNum(){return _displayNumber;}
char* SevenSeg::getDisplayText(){return _displayText;}
uint8_t SevenSeg::getDigitPin(const uint8_t index){return _digitPins[index];}
bool SevenSeg::getDisplayIsOn(){return _displayIsOn;}
bool SevenSeg::isOn(){return getDisplayIsOn();}
void SevenSeg::setDisplayIsOn(const bool isOn){_displayIsOn = isOn;}
uint8_t SevenSeg::getDecimalPosition(){return _decimalPosition;}
bool SevenSeg::getRawMode(){return _rawMode;}
void SevenSeg::setRawMode(const bool isRawMode){_rawMode = isRawMode;}
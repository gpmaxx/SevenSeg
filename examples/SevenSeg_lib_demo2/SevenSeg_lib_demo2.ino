#include <SevenSeg.h>
#include "pi.h"

const uint8_t DIGIT_PINS[] = {9,10,11,12};
const uint8_t DATA_PIN = A2;
const uint8_t LATCH_PIN = A1;
const uint8_t CLOCK_PIN = A0;
const uint8_t DISPLAY_NUM_DIGITS = 4;

uint32_t timer = millis();

SevenSeg display(DATA_PIN,LATCH_PIN,CLOCK_PIN,DISPLAY_NUM_DIGITS,DIGIT_PINS,HIGH,LOW);

void setup()
{
  Serial.begin(115200);
  display.on();
}

void loop()
{
  const uint16_t INIT_PAUSE = 2000;
  const uint16_t DIGIT_PAUSE = 100;
  
  static uint16_t len = strlen_P(PI_DIGITS);
  static uint8_t theDigit;
  uint16_t i = 4;

  display.setDisplayNum(3141,0); // set display to 3.141
  while ((millis() - timer) < INIT_PAUSE)
  {
    display.updateDisplay();
  }

  while (i < len)
  {
    if ((millis() - timer) > DIGIT_PAUSE)
    {
      theDigit = pgm_read_byte_near(PI_DIGITS + i) - '0';
      display.setDisplayDigitAndShift(theDigit);
      //Serial.print(theDigit);
      i++;
      timer = millis();
    }
    display.updateDisplay(); 
  }

  // start over
}




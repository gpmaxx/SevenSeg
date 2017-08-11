#include <SevenSeg.h>

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
  static uint16_t count = 0;
  if ((millis() - timer) > 1000)
  {
    count++;
    timer = millis();      
    display.setDisplayNum(count);
  }
  display.updateDisplay();
}



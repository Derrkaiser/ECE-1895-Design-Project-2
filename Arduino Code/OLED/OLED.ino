#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>



U8X8_SSD1306_128X64_NONAME_4W_SW_SPI OLED(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 11, /* reset=*/ 4); /* Constructor for OLED display */

//U8X8_SSD1306_128X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// Need cs to be 10 because pin 10 of arduino pin 10 is the slave select/chip select pin, can change reset to 4 and dc to another if needed


void setup() {
  /* Setup necessary pins */
  OLED.begin();


}

void loop() {
  //put your main code here, to run repeatedly:

  OLED.setFont(u8x8_font_chroma48medium8_r);
  OLED.drawString(0,0,"Hello World!");
  delay(1000);

}

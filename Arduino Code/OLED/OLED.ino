#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>



U8X8_SSD1306_128X64_NONAME_4W_SW_SPI OLED(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 8, /* reset=*/ 4); /* Constructor for OLED display */

char str[9];
float a = 10.50;

void setup() {
  /* Setup necessary pins */
  OLED.begin();
  OLED.setFont(u8x8_font_chroma48medium8_r);

}

void loop() {
  //put your main code here, to run repeatedly:

  u8x8.draw2x2String(1, 1, "Score:");
  OLED.setCursor(50,50);
  OLED.print(a);

  delay(5000);

}

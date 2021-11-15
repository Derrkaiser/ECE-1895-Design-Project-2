#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>



U8X8_SSD1306_128X64_NONAME_4W_SW_SPI OLED(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 8, /* reset=*/ 4); /* Constructor for OLED display */

char str[9];
uint8_t score = 10;

void setup() {
  /* Setup necessary pins */
  OLED.begin();
  OLED.setFont(u8x8_font_px437wyse700b_2x2_r);

}

void loop() {
  //put your main code here, to run repeatedly:

  OLED.drawString(0, 0, "Score:");
  OLED.setCursor(12,0);
  OLED.print(score);

  delay(3000);

  OLED.drawString(0,0,"You WIN!");
  OLED.drawString(0,4,"Play");
  OLED.drawString(0,6,"Again?");
  delay(3000);

  OLED.drawString(0,0,"You LOSE");
  
 

  OLED.drawString(0,4,"Play");
  OLED.drawString(0,6,"Again?");
  delay(3000);

  OLED.clear();
  OLED.drawString(0,0,"Press");
  OLED.drawString(0,2,"Start");
  OLED.drawString(0,4,"Button");
  OLED.drawString(0,6,"To Play");


  delay(3000);

  OLED.clear();

  




}

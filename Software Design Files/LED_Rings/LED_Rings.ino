#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
#include <U8x8lib.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        3 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 36 // Each ring has 12 pixels, and 3 rings = 36 total pixels. Address each ring with [0-11], [12-23], [24-35]

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN,NEO_RGBW + NEO_KHZ800);


#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
double colors[12] = {pixels.Color(255,255,0),pixels.Color(255,238,0),pixels.Color(255,221,0),
                    pixels.Color(255,204,0),pixels.Color(255,186,0),pixels.Color(255,168,0),
                    pixels.Color(255,149,0),pixels.Color(255,130,0),pixels.Color(255,109,0),
                    pixels.Color(255,86,0),pixels.Color(255,58,0),pixels.Color(255,0,0)};


void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:


pixels.fill(colors[2],0,12);
pixels.show();
delay(5000);
pixels.clear();
pixels.show();
delay(1000);
pixels.fill(colors[2],12,12);
pixels.show();
delay(5000);
pixels.clear();
pixels.show();
delay(1000);
pixels.fill(colors[2],24,12);
pixels.show();
delay(5000);
pixels.clear();
delay(1000);
//  double hit_strength = 1.0;
//  double decrement = hit_strength/12;;
//  double hit_voltage = hit_strength;
//  int i = 0;
//while(hit_voltage > 0)
//  {
//    pixels.setPixelColor(i,colors[i]);
//    hit_voltage = hit_voltage - decrement;
//    pixels.show();
//    delay(DELAYVAL);
//    i++;
//  }

  delay(5000);
}

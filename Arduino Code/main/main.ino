#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <U8x8lib.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        3 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12 // Each ring has 12 pixels, and 3 rings = 36 total pixels. Address each ring with [0-11], [12-23], [24-35]

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN,NEO_RGBW + NEO_KHZ800);


//U8X8_SSD1306_128X64_NONAME_4W_SW_SPI OLED(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 11, /* reset=*/ 4); /* Constructor for OLED display */


#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
double colors[12] = {pixels.Color(255,255,0),pixels.Color(255,238,0),pixels.Color(255,221,0),
                    pixels.Color(255,204,0),pixels.Color(255,186,0),pixels.Color(255,168,0),
                    pixels.Color(255,149,0),pixels.Color(255,130,0),pixels.Color(255,109,0),
                    pixels.Color(255,86,0),pixels.Color(255,58,0),pixels.Color(255,0,0)};



void setup() {
  /* Setup necessary pins */
  pinMode(3,OUTPUT);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();
  //OLED.begin();


}

void loop() {
  //put your main code here, to run repeatedly:

  
  OLED.setFont(u8x8_font_chroma48medium8_r);
  OLED.drawString(0,0,"Hello World!");
  delay(1000);






  /* Testing force sensor */
  

  /* While loop that waits for game to start up */

  /* Boot sequence (initalize display and turn on any LED's */

  /* Start Game, randomly select one of the actions and pad
   *  1. Hit selected pad with non-magnet side softly
   *  2. Hit selected pad with non-magnet side strongly
   *  3. Hit selected pad with magnet side softly
   *  4. Hit selected pad with magnet side strongly
   *  
   *  Pad is chosen using randn or equivalent function
   *  
   *  pad_choice = rannd(1-3)
   *  isStrong = randn(0-1)
   *  isMagnet_hit = randn(0-1) 0 = non-magnet, 1 = magnet
   *  
   *  
   *  Magnet vs Non-magnet and Hard vs Soft hit is chosen similarly
   *  Function is then called based on choice
   *  void magnet_hit(int pad_number, bool isStrong) 
   *    isStrong = 1 -> Strong hit
   *    isStrong = 0 -> Soft hit
   * 
   *  void non_magnet_hit(int pad_number, bool isStrong)
   */

}



void magnet_hit(int pad_number, bool isStrong)
{
  /* bool hit_detected = 0;
   *  
   *  Play audio cue
   *    audio_cue(game_choice);
   *  Turn on necessary lights of the pad
   *    toggle_pad_LED();
   *    
   *  Start timer for user to do the action
   *  
   */
  /* While loop to poll the pin corresponding to the pad number /*
    while(true)
    {

      if(timer has ended)
      {
        display that they lose
        break;
      }
      
      hit_voltage = analogRead(pad_pin)
      if(hit_voltage > 0)
      {
      magnet_used = digitalRead(magnet_sensor_pin);
        break;
      }

    }

    Player has hit the pad in time. Check to see if pad is within allowed
    threshold of force needed and if magnet was used
    if(hit_voltage > threshold of soft/strong || magnet_used == false)
    {
      display they lose
      update_display(lost);
    }
    else
    {
      continue game and go back to main
    }
      
   */
}

void non_magnet_hit(int pad_number, bool isStrong)
{
  /* bool hit_detected = 0;
   *  
   *  Play audio cue
   *    audio_cue(game_choice);
   *  Turn on necessary lights of the pad
   *    toggle_pad_LED();
   *    
   *  Start timer for user to do the action
   *  
   */
  /* While loop to poll the pin corresponding to the pad number /*
    while(true)
    {

      if(timer has ended)
      {
        display that they lose
        break;
      }
      
      hit_voltage = analogRead(pad_pin)
      if(hit_voltage > 0)
      {
        break;
      }

    }

    Player has hit the pad in time. Check to see if pad is within allowed
    threshold of force needed
    if(hit_voltage > threshold of soft/strong)
    {
      display they lose
      update_display(lost);
    }
    else
    {
      update_display(continue
      continue game and go back to main
    }
      
   */

}

/* Function to update the display. If the user lost, then update the display
 *  and turn off the game. If they did not lose, update the display score and continue the game
 *  */

void update_display(bool isLoser)
{
  /* 
   *  if(isLoser == true)
   *  {
   *    end the game and display that they lost
 *    }
 *    else(isLoser == false)
 *    {
 *      continue the game and update the score. 
 *      if the score is 99
 *      then end the game and say that they win
   */
}

/*  @param pad_number: The pad number to write data to
 *  
 *  @param hit_strength: How hard the pad was hit, used to determine how much of ring to light up
 */
void toggle_LED_ring(int pad_number, double hit_strength)
{
  /* Light up the LED corresponding to the selected pad_number
   *  digitalWrite(pad_number, HIGH);
   */

  // double hit_strength = 1.0;
  double decrement = hit_strength/12;;

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.

/* 
  0-11 - pad 1
  12-23 - pad 2
  24-35 - [ad 3
*/

  if(pad_number == 1)
  {
      int i = 0;
      while(hit_strength > 0)
      {
        pixels.setPixelColor(i,colors[i]);
        hit_strength = hit_strength - decrement;
        pixels.show();
        delay(DELAYVAL);
        i++;
      }
  }

  else if (pad_number == 2)
  {
      int i = 12;
      while(hit_strength > 0)
      {
        pixels.setPixelColor(i,colors[i-12]);
        hit_strength = hit_strength - decrement;
        pixels.show();
        delay(DELAYVAL);
        i++;
      }
  }

  else if(pad_number == 3)
  {
    int i = 24;
      while(hit_strength > 0)
      {
        pixels.setPixelColor(i,colors[i-24]);
        hit_strength = hit_strength - decrement;
        pixels.show();
        delay(DELAYVAL);
        i++;
      }
  }
}

void audio_cue(int pad_number, bool isStrong, bool hit_with_magnet)
{
  /* Depending on the input, output the correct audio cue
   *  i.e. if audio_cue(1, true, true)
   *  then output "Pad 1, strong, with magnet)
   *  
   *  Will store audio files on an SD card and play them depending on choice
   *  
   */
}

void boot_sequence()
{
  /* Turn on any necessary LED and start up the main display
   *  
   *  digitalWrite(LED, HIGH)
   *  
   *  write_to_display("Game on");
   *  also display the high score and any other display information wanted
   */
}

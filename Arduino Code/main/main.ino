#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SD.h>
#include <TMRpcm.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        3 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12 // Each ring has 12 pixels, and 3 rings = 36 total pixels. Address each ring with [0-11], [12-23], [24-35]

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

/* ----------------------------------------------------- OLED Display Related Variables and Constructors ----------------------------------------------------- */

/* OLED Display Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI OLED(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 8, /* reset=*/ 4);
/* 128 x 64 display = 16 columns x 8 rows */

int score = 0; /* Score for the user */


/* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */




/* ----------------------------------------------------- Sensor Related Variables and Constructors ----------------------------------------------------- */
/* FSR Pins */
#define FSR1  A3
#define FSR2  A4
#define FSR3  A5

/* Magnet Pins */
#define magnet1  A0
#define magnet2  A1
#define magnet3  A2

/* FRS Pins on Arduino Uno */
// #define FSR1  A2
// #define FSR2  A1
// #define FSR3  A0

/* Start Button Pin */
#define startButton 2

/* Global array to hold starting values of the force sensors at null input */
float force_threshold[3] = {0,0,0}; 

/* Global array to store the value read from sensor hits. Used to determine hit as well as the hitting of the wrong pad */
uint16_t force_read_voltage[3] = {0,0,0};

/* Global array to store the pin values for the FSR pads */
uint8_t fsr_pins[3] = {FSR1, FSR2, FSR3};

/* Global array to store the pin values for the FSR pads */
//uint8_t[3] magnet_pins = {magnet1, magnet2, magnet3};

/* Global array used to store analogRead() input and used to setup force_threshold calculation */
int force_read[3] = {0,0,0};

/* Variable used to add to force_threshold in order to limit accidental readings counting as hits */
float force_offset = 0.15;


/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */


// TODO: ADD TIMING TO poll_FSR. i.e. if they dont hit in time, then they lose 
void setup() {

  /* Start serial for Ardunio uno testing */
  Serial.begin(9600);

  /* Initialize OLED and display intro message */
  OLED.begin();
  OLED.setFont(u8x8_font_chroma48medium8_r);
  OLED.drawString(0,0,"Welcome to Bonk-It!");


  /* Read in sensor output and set value read in as the zero */
  for (unsigned short i = 0; i < 3; i++) {
    force_read_voltage[i] = analogRead(FSR1);
    force_threshold[i] = force_read_voltage[i] * (5.0 / 1023.0);
  }
  
  Serial.println(force_threshold[1]);
  Serial.println(force_threshold[2]);
  Serial.println(force_threshold[3]);

  while(digitalRead(startButton) != 1)
  {
    Serial.println("Waiting for start button");
    OLED.drawString(3,3,"Hit Start Button"); // TODO: Change location of where this outputs
  }
}

void loop() {
  //put your main code here, to run repeatedly:

  long random_pad = random(1,3); /* 1 = Pad 1, 2 = Pad 2, 3 = Pad 3 */
  bool hit_strength = random(0,1); /* 0 = weak, 1 = hard */
  bool magnet_hit = random(0,1); /* 0 = non-magnet, 1 = magnet */

  uint8_t pad_returned = 0; /* Which pad was hit, returned from poll_FSR */

  unsigned long time = millis();
  unsigned long timeout = 3000 - 25 * score - time;

  while(millis() <= timeout && !pad_returned)
    pad_returned = poll_FSR(random_pad); /* Poll the FSR's */

  if (millis() > timeout)
    game_over();

  /* If non-magnetic hit */
  if(!magnet_hit)
  {
    if(pad_returned == random_pad) /* Correct pad was hit */
    {
      /* Check if correct strength */
      if(check_correct_hit_strength(pad_returned) == true)
      {
        /* Update the score and display and proceed to next round */
        score++;
        update_display();
        return; /* Go back to the start of the void loop() function */
      }
      else /* Incorrect strength, the user loses */
      {
        /* Update the display and issue game-over */
        game_over();
      }
    }
    
  }
  
  
  /* If magnetic hit */
  else
  {
    
  }
  /* Start Game, randomly select one of the actions and pad
   *  1. Hit selected pad with non-magnet side softly
   *  2. Hit selected pad with non-magnet side strongly
   *  3. Hit selected pad with magnet side softly
   *  4. Hit selected pad with magnet side strongly
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

void update_display()
{
  OLED.clear();
  OLED.drawString(5,5,"Score:"); //TODO CHANGE FONT SIZE
  OLED.setCursor(10,10); //TODO: CHANGE LOCATION OF CURSOR
  OLED.print(score);
}

void game_over()
{
  OLED.clear();
  OLED.drawString(5,5,"YOU LOSE"); //TODO CHANGE FONT SIZE
  OLED.drawString(10,10, "Score: "); //TODO: CHANGE LOCATION OF CURSOR AND SIZE
  OLED.setCursor(10,16); //TODO: CHANGE LOCATION OF CURSOR
  OLED.print(score);

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

void audio_cue(uint8_t pad_number, bool isStrong, bool hit_with_magnet)
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

/* Function to poll the FSR's and return which one was hit */
uint8_t poll_FSR(uint8_t pad)
{
  while(true)
  {
    /* Read in from all of the pins */
    force_read[1] = analogRead(fsr_pins[1]);
    force_read[2] = analogRead(fsr_pins[2]);
    force_read[3] = analogRead(fsr_pins[3]);

    /* Convert values to voltages */
    force_read_voltage[1] = force_read[1] * (5.0 / 1023.0);
    force_read_voltage[2] = force_read[2] * (5.0 / 1023.0);
    force_read_voltage[3] = force_read[3] * (5.0 / 1023.0);

    /* Check if any of the read voltages surpass the threshold for its pad, if so return that the pad was hit */
    if(force_read_voltage[1] > force_threshold[1] + force_offset)
    {
      return 1; /* Pad 1 was hit */
    }
    else if (force_read_voltage[2] > force_threshold[2]+ force_offset)
    {
      return 2; /* Pad 2 was hit */
    }
    else if (force_read_voltage[3] > force_threshold[3] + force_offset)
    {
      return 3; /* Pad 3 was hit */
    }
    else
    {
      return 0; /* Shouldn't occur */
    }

  }  
}

/* Helper function to check if the pad was hit with the correct strength */
long check_correct_hit_strength(uint8_t pad_number)
{
  
}

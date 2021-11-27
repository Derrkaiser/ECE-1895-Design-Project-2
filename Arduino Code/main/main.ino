#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
#include <U8x8lib.h>



#define debugMode 1 /* 1 for serial monitor output, 0 for no debug */


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

uint8_t score = 0; /* Score for the user */


/* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */




/* ----------------------------------------------------- Sensor Related Variables and Constructors ----------------------------------------------------- */

#if debugMode
  /* FRS Pins on Arduino Uno */
  #define FSR1  A2
  #define FSR2  A1
  #define FSR3  A0
#else
  /* FSR Pins */
  #define FSR1  A3
  #define FSR2  A4
  #define FSR3  A5
#endif

uint8_t FSR[3] = {FSR1, FSR2, FSR3};

/* Magnet Pins */
#define magnet1  A0
#define magnet2  A1
#define magnet3  A2


/* Start Button Pin */
#define startButton 2

/* Global array to hold starting values of the force sensors at null input */
float force_threshold[3]; 


/* Global array used to store analogRead() input and used to setup force_threshold calculation */
uint16_t force_read[3] = {0,0,0};

/* Global array to store the value read from sensor hits. Used to determine hit as well as the hitting of the wrong pad */
float force_read_voltage[3] = {0,0,0};

/* Global array to store the pin values for the FSR pads */
uint8_t fsr_pins[3] = {FSR1, FSR2, FSR3};

/* Global array to store the pin values for the magnets */
uint8_t magnet_pins[3] = {magnet1, magnet2, magnet3};

/* Global array to store digitalRead() input for magnets */
uint8_t magnet_read[3] = {0,0,0};

/* Variable used to add to force_threshold in order to limit accidental readings counting as hits */
float force_offset = 0.15;

/* Global array to store the threshold for where the weak/hit threshold is */
float hit_strength_threshold[3];


/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */

/* Variable and pin for SD Card/Audio setup */
#define SD_ChipSelectPin 10
TMRpcm tmrpcm;


void setup() {

#if debugMode
  /* Start serial for Ardunio uno testing */
  Serial.begin(9600);
#endif

  /* Initialize OLED and display intro message */
  OLED.begin();
  OLED.setFont(u8x8_font_px437wyse700b_2x2_r);
  OLED.drawString(0,0,"Welcome");
  OLED.drawString(0,2,"to");
  OLED.drawString(0,4,"Bonk-It!");

  sd_card_setup();
  
  initialize_pads();
  
  boot_sequence(); // DOES NOTHING AS OF NOW

#if debugMode
  Serial.println("Waiting for start button");
#endif

  OLED.clear();
  OLED.drawString(0,0,"Press");
  OLED.drawString(0,2,"Start");
  OLED.drawString(0,4,"Button");
  OLED.drawString(0,6,"To Play");
}

void loop() {
  //TODO: Check to see if want this to display when people play again
//  OLED.clear();
//  OLED.drawString(0,0,"Press");
//  OLED.drawString(0,2,"Start");
//  OLED.drawString(0,4,"Button");
//  OLED.drawString(0,6,"To Play");
  if (digitalRead(startButton))
  {
    play_game();
  }
}


void play_game() {
  // Declaring round variables that are updated each loop
  uint8_t random_pad;
  bool hit_strength, magnet_hit;
  unsigned long time, timeout;
  uint8_t pad_returned;

  while (score < 100) {
    // Update round variables
    random_pad = random(1,3); /* 1 = Pad 1, 2 = Pad 2, 3 = Pad 3 */
    hit_strength = random(0,1); /* 0 = weak, 1 = hard */
    magnet_hit = random(0,1); /* 0 = non-magnet, 1 = magnet */
    pad_returned = 0; /* Which pad was hit, returned from poll_FSR */

    show_task(random_pad, hit_strength, magnet_hit);
    audio_cue(random_pad, hit_strength, magnet_hit);

    // Setup timer
    time = millis();
    timeout = 3000 - 25 * score - time;

    // Run timeout timer and detect input
    while(millis() <= timeout && !pad_returned)
      pad_returned = poll_FSR(); /* Poll the FSR's */

    // End game if too much time passed
    if (millis() > timeout)
      break;

    /* If non-magnetic hit */
    if(!magnet_hit)
    {
      if(pad_returned == random_pad) /* Correct pad was hit */
      {
        toggle_LED_ring(pad_returned);
        /* Check if correct strength */
        if(check_correct_hit_strength(pad_returned, hit_strength))
        {
          /* Update the score and display and proceed to next round */
          score++;
          show_score(false); //true => game continues and show score
          continue; /* Go back to the start of the while loop function */
        }
        else /* Incorrect strength, the user loses */
        {
          /* Breaks loop and executes game_over() */
          break;
        }
      }
    }
    
    /* If magnetic hit */
    else
    {
      if(pad_returned == random_pad) /* Correct pad was hit */
      {
        toggle_LED_ring(pad_returned);
        /* Check if correct strength */
        if(check_correct_hit_strength(pad_returned, hit_strength))
        {
          /* Check if used magnet side */
          if(magnet_read[pad_returned] == 1)
          {
            /* Update the score and display and proceed to next round */
            score++;
            show_score(false); //true => game continues and show score
            continue; /* Go back to the start of the while loop function */
          }
          else
          {
            /* Didnt use magnet, the user loses */
            break;
          }
          
        }
        else /* Incorrect strength, the user loses */
        {
          /* Breaks loop and executes game_over() */
          break;
        }
      }
    
    }
  }

  // Win only if score is 99 or more
  (score < 99) ? game_over() : win();
  return;
}



void game_over()
{
  OLED.clear();
  OLED.drawString(0,0,"You LOSE");
  show_score(true); // show_score(true) means they lost
  play_again(); 
}

void win() {
  OLED.clear();
  OLED.drawString(0,0,"You WIN!");
  play_again();
}

void play_again()
{
    /* Reset the game for next run through */
    delay(5000); 
    score = 0;
    initialize_pads();
    OLED.clear();
    OLED.drawString(0,4,"Play");
    OLED.drawString(0,6,"Again?");
    /* Function returns to play_game which then immediately returns to loop() */
}

// Helper function to show the score on the OLED
void show_score(bool lost) 
{
  if(lost == true)
  {
    OLED.drawString(0,2, "Score: "); 
    OLED.setCursor(12,2);
    OLED.print(score);
  }
  else /* Game still in progress */
  {
    OLED.clear();
    OLED.drawString(0,0, "Score: "); 
    OLED.setCursor(12,0);
    OLED.print(score);
  }
}


/*  @param pad_number: The pad number to write data to
 *  
 *  @param hit_strength: How hard the pad was hit, used to determine how much of ring to light up
 */
void toggle_LED_ring(uint8_t pad_number)
{
  /* Light up the LED corresponding to the selected pad_number
   *  digitalWrite(pad_number, HIGH);
   */

  // Exit if pad number is 0
  if (!pad_number) return;

  // double hit_strength = 1.0;
  //double decrement = hit_strength/12;;
  double decrement = 0.41; /* 5 Volts / 12 LED's */
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.

  /* 
    0-11 - pad 1
    12-23 - pad 2
    24-35 - [ad 3
  */

  uint8_t i = (pad_number - 1) * 12;
  uint8_t pad_index = i;
  double hit_voltage;
  hit_voltage = force_read_voltage[pad_number];
  while(hit_voltage > 0)
  {
    pixels.setPixelColor(i,colors[i - pad_index]);
    hit_voltage = hit_voltage - decrement;
    pixels.show();
    delay(DELAYVAL);
    i++;
  }
}

void audio_cue(uint8_t pad_number, bool isStrong, bool hit_with_magnet)
{
  String file = "";
  file.concat(pad_number);

  if(isStrong == true)
  {
    file.concat("_strong");
  }
  else
  {
    file.concat("_weak");
  }

  if(hit_with_magnet == true)
  {
    file.concat("_magnet.WAV");
  }
  else
  {
    file.concat("_non-magnet.WAV");
  }

  tmrpcm.play(file.c_str());

}

void boot_sequence()
{

}
void initialize_pads()
{
  /* Read in sensor output and set value read in as the zero */
    for (uint8_t i = 0; i < 3; i++) {
      force_read[i] = analogRead(FSR[i]);
      force_threshold[i] = force_read[i] * (5.0 / 1023.0);
      hit_strength_threshold[i] = force_threshold[i] + 1.5; //TODO Check the offset with testing to see where soft/hard hit should end
      
    }

#if debugMode    
    Serial.println(force_threshold[1]);
    Serial.println(force_threshold[2]);
    Serial.println(force_threshold[3]);
#endif
}

/* Function to poll the FSR's and return which one was hit */
uint8_t poll_FSR()
{
  for (uint8_t i = 0; i < 3; i++) {
    /* Read in from all of the pins */
    force_read[i] = analogRead(fsr_pins[i]);

    /* Convert values to voltages */
    force_read_voltage[i] = force_read[i] * (5.0 / 1023.0);

    /* Check if any of the read voltages surpass the threshold for its pad, if so return that the pad was hit */
    if(force_read_voltage[i] > force_threshold[i] + force_offset)
      return i + 1;
  }

  // No pad was hit
  return 0;
}

/* Function to poll the FSR's and magnets and return which one was hit */
uint8_t poll_FSR_with_magnet()
{
  for (uint8_t i = 0; i < 3; i++) {
    /* Read in from all of the pins */
    force_read[i] = analogRead(fsr_pins[i]);
    magnet_read[i] = digitalRead(magnet_pins[i]);

    /* Convert values to voltages */
    force_read_voltage[i] = force_read[i] * (5.0 / 1023.0);

    /* Check if any of the read voltages surpass the threshold for its pad, if so return that the pad was hit */
    if(force_read_voltage[i] > force_threshold[i] + force_offset)
      return i + 1;
  }

  // No pad was hit
  return 0;
}

/* Helper function to check if the pad was hit with the correct strength */
bool check_correct_hit_strength(uint8_t pad_number, uint8_t hit_strength)
{
  /* hit_strength = 0 => weak
   * hit_strength = 1 => hard
   */
  if(hit_strength == 1)
  {
    if(force_read_voltage[pad_number] >= hit_strength_threshold[pad_number])
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if(hit_strength == 0)
  {
    if(force_read_voltage[pad_number] < hit_strength_threshold[pad_number])
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

void show_task(uint8_t pad_number, bool hit_strength, bool magnet_hit)
{
    OLED.clear();
    OLED.drawString(0,2, "Pad: "); 
    OLED.setCursor(8,2);
    OLED.print(pad_number);

    if(hit_strength == 0)
    {
      OLED.drawString(0,4,"Weak");
    }
    else
    {
      OLED.drawString(0,4,"Strong");

    }

    if(magnet_hit == 1)
    {
      OLED.drawString(0,6,"Magnet");
    }
    else
    {
      OLED.drawString(0,6,"No-Magnet");
    }
}

void sd_card_setup()
{
    tmrpcm.speakerPin=9; // 9 for Uno, Nano, etc.
    SD.begin(SD_ChipSelectPin);
    tmrpcm.setVolume(7);
}

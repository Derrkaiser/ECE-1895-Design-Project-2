#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
#include <U8x8lib.h>



#define debugMode 1 /* 1 for serial monitor output, 0 for no debug */


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

/* ----------------------------------------------------- OLED Display Related Variables and Constructors ----------------------------------------------------- */

/* OLED Display Constructor */
U8X8_SSD1306_128X64_NONAME_4W_HW_SPI OLED(/* cs=*/ 7, /* dc=*/ 8, /* reset=*/ 4); /* Constructor for OLED display */
/* 128 x 64 display = 16 columns x 8 rows */

uint8_t score = 0; /* Score for the user */


/* ----------------------------------------------------------------------------------------------------------------------------------------------------------- */




/* ----------------------------------------------------- Sensor Related Variables and Constructors ----------------------------------------------------- */


/* FSR Pins */
#define FSR1  A0
#define FSR2  A1
#define FSR3  A2

uint8_t FSR[3] = {FSR1, FSR2, FSR3};

/* Magnet Pins */
#define magnet1  A3
#define magnet2  A4
#define magnet3  A5


/* Start Button Pin */
#define startButton 2

/* Global array to hold starting values of the force sensors at null input */
float force_threshold[3] = {0,0,0}; 


/* Global array used to store analogRead() input and used to setup force_threshold calculation */
uint16_t force_read[3] = {0,0,0};

/* Global array to store the value read from sensor hits. Used to determine hit as well as the hitting of the wrong pad */
float force_read_voltage[3] = {0,0,0};

/* Global array to store the pin values for the FSR pads */
uint8_t fsr_pins[3] = {FSR1, FSR2, FSR3};

/* Global array to store the pin values for the magnets */
uint8_t magnet_pins[3] = {magnet1, magnet2, magnet3};

/* Global array to store analogRead() input for magnets */
uint16_t magnet_read[3] = {0,0,0};

/* Variable used to add to force_threshold in order to limit accidental readings counting as hits */
float force_offset = 0.15;

/* Global array to store the threshold for where the weak/hit threshold is */
float hit_strength_threshold[3] = {0,0,0};


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

  randomSeed(A0);

  delay(5000);

  pixels.begin();
  pixels.show();


  //sd_card_setup();
  
  initialize_pads();
  
  boot_sequence(); // DOES NOTHING AS OF NOW

  pinMode(2,INPUT); /* Start Button */

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
  uint8_t hit_strength, magnet_hit;
  unsigned long time_t, timeout;
  uint8_t pad_returned;

  timeout = millis();

  while (score < 100) {
    // Update round variables
    random_pad = random(1,4); /* 1 = Pad 1, 2 = Pad 2, 3 = Pad 3 */
    hit_strength = random(0,2); /* 0 = weak, 1 = hard */
    magnet_hit = random(0,2); /* 0 = non-magnet, 1 = magnet */
    pad_returned = 0; /* Which pad was hit, returned from poll_FSR */

    Serial.print("random_pad = ");
    Serial.println(random_pad);

    Serial.print("hit_strength = ");
    Serial.println(hit_strength);

    Serial.print("magnet_hit = ");
    Serial.println(magnet_hit);

    reset_pads();

    

    pixels.clear();
    pixels.show();
    show_task(random_pad, hit_strength, magnet_hit);
    delay(1000);
    //audio_cue(random_pad, hit_strength, magnet_hit);

    // Setup timer
    time_t = millis();
    Serial.print("Time_T = ");
    Serial.println(time_t);
    Serial.println(time_t/1000);
    
    timeout = 3000 - (25 * score);

    // Run timeout timer and detect input

    if(magnet_hit == 0)
    {
    while(millis() < time_t + timeout && pad_returned == 0)
      
      pad_returned = poll_FSR(); /* Poll the FSR's */
      time_t = millis();
      
    }
    else
    {
      while(millis() < time_t + timeout && pad_returned == 0)
      pad_returned = poll_FSR_with_magnet(); /* Poll the FSR's */

      time_t = millis();
    }

    // End game if too much time passed
    if (timeout <= 0)
      break;

    /* If non-magnetic hit */
    if(magnet_hit == 0)
    {
              toggle_LED_ring(pad_returned);

      if(pad_returned == random_pad) /* Correct pad was hit */
      {

        Serial.print("Pin 0 = ");
        Serial.println(force_read_voltage[0]);
        Serial.print("Pin 1 = ");
        Serial.println(force_read_voltage[1]);
        Serial.print("Pin 2 = ");
        Serial.println(force_read_voltage[2]);

        
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
      else
      {
        Serial.print("Pin 0 = ");
        Serial.println(force_read_voltage[0]);
        Serial.print("Pin 1 = ");
        Serial.println(force_read_voltage[1]);
        Serial.print("Pin 2 = ");
        Serial.println(force_read_voltage[2]);

        break;
      }
    }
    
    /* If magnetic hit */
    else
    {
      toggle_LED_ring(pad_returned);

      Serial.println("Magnet Hit");
      Serial.print("Pad hit = ");
      Serial.println(pad_returned);
      Serial.print("random pad = ");
      Serial.println(random_pad);

      if(pad_returned == random_pad) /* Correct pad was hit */
      {
        Serial.print("Pin 0 = ");
        Serial.println(force_read_voltage[0]);
        Serial.print("Pin 1 = ");
        Serial.println(force_read_voltage[1]);
        Serial.print("Pin 2 = ");
        Serial.println(force_read_voltage[2]);

        

        /* Check if correct strength */
        if(check_correct_hit_strength(pad_returned, hit_strength))
        {
          Serial.print("Magnet = ");
          Serial.println(magnet_read[pad_returned]);
          /* Check if used magnet side */
//          if(magnet_read[pad_returned] != 0)
//          {
            /* Update the score and display and proceed to next round */
            score++;
            show_score(false); //true => game continues and show score
            continue; /* Go back to the start of the while loop function */
//          }
//          else
//          {
//            /* Didnt use magnet, the user loses */
//            break;
//          }
          
        }
        else /* Incorrect strength, the user loses */
        {
          /* Breaks loop and executes game_over() */
          break;
        }
      }
      else
      {
        Serial.print("Pin 0 = ");
        Serial.println(force_read_voltage[0]);
        Serial.print("Pin 1 = ");
        Serial.println(force_read_voltage[1]);
        Serial.print("Pin 2 = ");
        Serial.println(force_read_voltage[2]);
        break;
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
    loop();
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
    delay(5000);
  }
  else /* Game still in progress */
  {
    OLED.clear();
    OLED.drawString(0,0, "Score: "); 
    OLED.setCursor(12,0);
    OLED.print(score);

    pixels.clear();

    delay(5000);
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

if(pad_number == 1)
{
  pixels.clear();
  pixels.show();
  pixels.fill(colors[2],0,12);
  pixels.show();
}
else if (pad_number == 2)
{
  pixels.clear();
  pixels.show();
  pixels.fill(colors[2],12,12);
  pixels.show();
}
else if(pad_number == 3)
{
  pixels.clear();
  pixels.show();
  pixels.fill(colors[2],24,12);
  pixels.show();
}

//  // Exit if pad number is 0
//  if (pad_number == 0) return;
//
//  // double hit_strength = 1.0;
//  //double decrement = hit_strength/12;;
//  double decrement = 0.05;//0.41; /* 5 Volts / 12 LED's */
//  // The first NeoPixel in a strand is #0, second is 1, all the way up
//  // to the count of pixels minus one.
//
//  /* 
//    0-11 - pad 1
//    12-23 - pad 2
//    24-35 - [ad 3
//  */
//
//  uint8_t i = (pad_number - 1) * 12;
//  uint8_t pad_index = i;
//  double hit_voltage;
//  hit_voltage = force_read_voltage[pad_number];
//  while(hit_voltage > 0)
//  {
//    pixels.setPixelColor(i,colors[i - pad_index]);
//    hit_voltage = hit_voltage - decrement;
//    pixels.show();
//    delay(DELAYVAL);
//    i++;
//  }
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
      hit_strength_threshold[i] = force_threshold[i] + 0.3; //TODO Check the offset with testing to see where soft/hard hit should end
//
//      force_threshold[i] = 0.15;
//      hit_strength_threshold[i] = 0.2;
      
    }

    

#if debugMode    
    Serial.println("Force Thresholds");
    Serial.println(force_threshold[1]);
    Serial.println(force_threshold[2]);
    Serial.println(force_threshold[3]);

    Serial.println("Hit_strength_thresholds");
    Serial.println(hit_strength_threshold[1]);
    Serial.println(hit_strength_threshold[2]);
    Serial.println(hit_strength_threshold[3]);
#endif
}

/* Function to poll the FSR's and return which one was hit */
uint8_t poll_FSR()
{
  for (uint8_t i = 0; i < 3; i++) {
    /* Read in from all of the pins */
    force_read[i] = analogRead(fsr_pins[i]);
//    Serial.print("Pin 0 = ");
//    Serial.println(force_read[0]);
//    Serial.print("Pin 1 = ");
//    Serial.println(force_read[1]);
//    Serial.print("Pin 2 = ");
//    Serial.println(force_read[2]);

    /* Convert values to voltages */
    force_read_voltage[i] = force_read[i] * (5.0 / 1023.0);



    /* Check if any of the read voltages surpass the threshold for its pad, if so return that the pad was hit */
    if(force_read_voltage[i] > force_threshold[i] + force_offset)
    {
      return i + 1;
      Serial.print("Pad hit = ");
      Serial.println(i+1);
    }
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
    magnet_read[i] = analogRead(magnet_pins[i]);

    Serial.println(magnet_read[i]);

    /* Convert values to voltages */
    force_read_voltage[i] = force_read[i] * (5.0 / 1023.0);

    /* Check if any of the read voltages surpass the threshold for its pad, if so return that the pad was hit */
    if(force_read_voltage[i] > force_threshold[i] + force_offset)
    {
      Serial.print("Pad hit = ");
      Serial.println(i+1);
      return i + 1;
    }
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
    if(force_read_voltage[pad_number-1] >= hit_strength_threshold[pad_number-1])
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
    if(force_read_voltage[pad_number-1] < hit_strength_threshold[pad_number-1])
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

void show_task(uint8_t pad_number, uint8_t hit_strength, uint8_t magnet_hit)
{
    OLED.clear();
    OLED.drawString(0,2, "Pad "); 
    OLED.setCursor(6,2);
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
      OLED.drawString(0,6,"Gold");
    }
    else
    {
      OLED.drawString(0,6,"Black");
    }
}

void sd_card_setup()
{
    tmrpcm.speakerPin=9; // 9 for Uno, Nano, etc.
    SD.begin(SD_ChipSelectPin);
    tmrpcm.setVolume(7);
}

void reset_pads()
{
  force_read_voltage[0] = 0;
  force_read_voltage[1] = 0;
  force_read_voltage[2] = 0;

  force_read[0] = 0;
  force_read[1] = 0;
  force_read[2] = 0;
}

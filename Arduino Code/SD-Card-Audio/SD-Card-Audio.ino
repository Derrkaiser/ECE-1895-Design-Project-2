#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

#define SD_ChipSelectPin 10


TMRpcm tmrpcm;

void setup(void) {
  tmrpcm.speakerPin=9; // 9 for Uno, Nano, etc.
  
  Serial.begin(9600);
  while(!Serial){;}
  
  while(!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");  
  }
  
  Serial.println("SD Success");


//  //Reads the chip
//  SD.begin(SD_ChipSelectPin);
//
  //Sets the volume 
  tmrpcm.setVolume(8);
}

void loop(void) {

  if(Serial.available()){    
    if(Serial.read() == 'p'){ //send the letter p over the serial monitor to start playback
      Serial.println("Playback start");
      tmrpcm.play("test3.WAV");
    }
  }
//  tmrpcm.play("test.wav");
//  delay(5000);
}

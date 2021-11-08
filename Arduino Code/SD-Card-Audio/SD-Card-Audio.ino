#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

#define SD_ChipSelectPin 10


TMRpcm tmrpcm;

void setup(void) {
 
  tmrpcm.speakerPin=9; // 9 for Uno, Nano, etc.

  //Reads the chip
  SD.begin(SD_ChipSelectPin);

  //Sets the volume 
  tmrpcm.setVolume(6);
}

void loop(void) {
  tmrpcm.play("test.wav");
  delay(5000);
}

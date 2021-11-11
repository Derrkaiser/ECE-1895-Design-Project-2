#include "SPI.h"


#define magnet1  A0
#define magnet2  A1
#define magnet3  A2

//#define FSR1  A3
//#define FSR2  A4
//#define FSR3  A5

#define FSR1  A2
#define FSR2  A1
#define FSR3  A0
/* On arduino uno, FSR1 = A2, FSR2 = A1, FSR3 = A0 */

double magnet_threshold;

float force_threshold[3] = {5,5,5};

float force_read_voltage;


void setup() {
  // put your setup code here, to run once:
  pinMode(8,OUTPUT); /* Used for testing, lights up a LED */
  Serial.begin(9600);
  int force_read_1;
  int force_read_2;
  int force_read_3;

  force_read_1 = analogRead(FSR1);
  force_threshold[1] = force_read_1 * (5.0 / 1023.0);
  Serial.println(force_threshold[1]);
  
  force_read_2 = analogRead(FSR2);
  force_threshold[2] = force_read_2 * (5.0 / 1023.0);
  Serial.println(force_threshold[2]);

  
  force_read_3 = analogRead(FSR3);
  force_threshold[3] = force_read_3 * (5.0 / 1023.0);
  Serial.println(force_threshold[3]);
}

void loop() {
  // put your main code here, to run repeatedly


  //test_magnets();

  test_FSR();

}

//void test_magnets() 
//{
//  
//  if(poll_magnets(A0) == 1)
//  {
//    digitalWrite(8,HIGH);
//  }
//
//  delay(5000);
//  digitalWrite(8,LOW);
//  
//  if(poll_magnets(A0) == 1)
//  {
//    digitalWrite(8,HIGH);
//  }
//  
//  delay(5000);
//  digitalWrite(8,LOW);
//
//  if(poll_magnets(A0) == 1)
//  {
//    digitalWrite(8,HIGH);
//  }
//
//  delay(5000);
//  digitalWrite(8,LOW);
//  delay(5000);
//}
//

void test_FSR() 
{
  
  if(poll_FSR(FSR1,1) == 1)
  {
       Serial.print("Pad number 1 reads ");
       Serial.println(force_read_voltage);
  }

  delay(2000);


  if(poll_FSR(FSR2,2) == 1)
  {
       Serial.print("Pad number 2 reads ");
       Serial.println(force_read_voltage);
  }

  delay(2000);
  
  if(poll_FSR(FSR3,3) == 1)
  {
       Serial.print("Pad number 3 reads ");
       Serial.println(force_read_voltage);
  }

  delay(2000);
}

//bool poll_magnets(int magnet_number)
//{
//  while(true)
//  {
//    if(analogRead(magnet_number) >= magnet_threshold)
//    {
//      return true;
//    }
//  } 
//}
//
bool poll_FSR(uint8_t fsr_pin, int pad_number)
{
  
  int force_read;
  while(true)
  {
    force_read = analogRead(fsr_pin);
    force_read_voltage = force_read * (5.0 / 1023.0);
    if(force_read_voltage > force_threshold[pad_number] + 0.15)
    {
      return true;
    }
  } 
//  switch(pad_number){
//    case 1:
//      while(true)
//      {
//        force_read = analogRead(FSR1);
//        force_read_voltage = force_read * (5.0 / 1023.0);
//        if(force_read > force_threshold[pad_number])
//        {
//          return true;
//        }
//      } 
//      break;
//    case 2:
//      while(true)
//      {
//        force_read = analogRead(FSR2);
//        force_read_voltage = force_read * (5.0 / 1023.0);
//        if(force_read > force_threshold[pad_number])
//        {
//          return true;
//        }
//      } 
//      break;
//    case 3:
//      while(true)
//      {
//        force_read = analogRead(FSR3);
//        force_read_voltage = force_read * (5.0 / 1023.0);
//        if(force_read > force_threshold[pad_number])
//        {
//          return true;
//        }
//      } 
//      break;
//  }
      
    
    
}

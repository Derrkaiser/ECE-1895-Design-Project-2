void setup() {
 /* Three LED's */
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);

  /* Power Button for LED */
  pinMode(9, INPUT);

 /* Reset Button for Chip */
  pinMode(1, INPUT); 

}

void loop() {
  // put your main code here, to run repeatedly:

  //light_single_LED();
  light_three_LED();

}

void light_single_LED()
{
  while(digitalRead(9) == HIGH) /* While button pressed */
  {
    digitalWrite(8,HIGH);
    delay(500); /* Keep on for 1/2 second */
    digitalWrite(8,LOW);
    delay(500); /* Keep off for 1/2 second */
  }
}

void light_three_LED()
{
  while(digitalRead(9) == HIGH) /* While button pressed */
  {
    /* Turn 1st LED on/off */
    digitalWrite(3,HIGH);
    delay(500); /* Keep on for 1/2 second */
    digitalWrite(3,LOW);
    delay(250);

    /* Turn 2nd LED on/off */
    digitalWrite(4,HIGH);
    delay(500); /* Keep on for 1/2 second */
    digitalWrite(4,LOW);
    delay(250);

    /* Turn 3rd LED on/off */
    digitalWrite(8,HIGH);
    delay(500); /* Keep on for 1/2 second */
    digitalWrite(8,LOW);
    delay(250);

    
  }
}

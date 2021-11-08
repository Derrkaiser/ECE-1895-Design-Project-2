int magnet1 = A0;
int magnet2 = A1;
int magnet3 = A2;

double magnet_threshold = 0.1;
double force_threshold = 0.1;

void setup() {
  // put your setup code here, to run once:
  pinMode(8,OUTPUT); /* Used for testing, lights up a LED */
}

void loop() {
  // put your main code here, to run repeatedly:


  test_magnets();

  //test_FSR();

}

void test_magnets() 
{
  
  if(poll_magnets(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }

  delay(5000);
  digitalWrite(8,LOW);
  
  if(poll_magnets(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }
  
  delay(5000);
  digitalWrite(8,LOW);

  if(poll_magnets(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }

  delay(5000);
  digitalWrite(8,LOW);
  delay(5000);
}

void test_FSR() 
{
  
  if(poll_FSR(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }

  delay(5000);
  digitalWrite(8,LOW);
  
  if(poll_FSR(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }
  
  delay(5000);
  digitalWrite(8,LOW);

  if(poll_FSR(A0) == 1)
  {
    digitalWrite(8,HIGH);
  }

  delay(5000);
  digitalWrite(8,LOW);
  delay(5000);
}

bool poll_magnets(int magnet_number)
{
  while(true)
  {
    if(analogRead(magnet_number) >= magnet_threshold)
    {
      return true;
    }
  } 
}

bool poll_FSR(int pad_number)
{
  while(true)
  {
    if(analogRead(pad_number) >= force_threshold)
    {
      return true;
    }
  } 
}

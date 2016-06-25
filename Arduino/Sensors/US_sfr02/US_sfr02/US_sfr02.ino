

#include <Wire.h>

int reading = 0;
double sonarTimer= 0;
long int contSonarRoutine= 0;
long int maxsonarTimer = 0;
double sonarTimeTot =0;

byte lowB,highB;

void setup()
{
  Wire.begin();                
  Serial.begin(9600);
  Serial.println("OK");

  setupSfr02();
}

void setupSfr02()
{

  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(112); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x51));      // command sensor to measure in "cm" (0x51)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting
  
}
void loop()
{
  delay(70);                   

  Wire.beginTransmission(112); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting
  
  sonarTimer = micros();
  Wire.requestFrom(112, 2);    // request 2 bytes from slave device #112

  // step 5: receive reading from sensor
  if (2 <= Wire.available())   // if two bytes were received
  {
    highB = Wire.read();  // receive high byte (overwrites previous reading)
    lowB = Wire.read(); // receive low byte as lower 8 bits
    reading = (highB << 8) + lowB;
    Serial.print(reading);   // print the reading
    Serial.println("cm");
  }


  contSonarRoutine++;  
  sonarTimer = micros() - sonarTimer;
  if (maxsonarTimer <= sonarTimer)
    maxsonarTimer = sonarTimer;
  sonarTimeTot = sonarTimeTot + sonarTimer;
    //Serial.print(sonarTimer);   // print the reading

  delay(1000);                  // wait a bit since people have to read the output :)
}

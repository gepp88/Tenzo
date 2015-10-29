int E1 = 3;     //M1 Speed Control
int E2 = 11;     //M2 Speed Control
int M1 = 4;     //M1 Direction Control
int M2 = 7;     //M1 Direction Control
int counter=0;
 
int speed1 = 0,speed2 = 0; 
 
boolean verbose = true; 
 
void stop(void)                    //Stop
{
  digitalWrite(E1,0); 
  digitalWrite(M1,LOW);    
  digitalWrite(E2,0);   
  digitalWrite(M2,LOW);    
}   
void advance(char a,char b)          //Move forward
{
  analogWrite (E1,a);      //PWM Speed Control
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  

void advanceM1(char a)          //Move forward
{
  analogWrite (E1,a);      
  digitalWrite(M1,HIGH);
}  

void advanceM2(char b)          //Move forward
{
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  

void back_off (char a,char b)          //Move backward
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void turn_L (char a,char b)             //Turn Left
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}
void turn_R (char a,char b)             //Turn Right
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

void current_sense()                  // current sense and diagnosis
{
  int val1=digitalRead(2);            
  int val2=digitalRead(3);
  if(val1==HIGH || val2==HIGH){
    counter++;
    if(counter==3){
      counter=0;
      Serial.println("Warning");
    }  
  } 
}

void setupTimers()
{
  // Timer settings
  // Initialize Timer
  
  
  cli();
  
  // Motor 1:
  
  TCCR2A = 0;
  TCCR2B = 0;

  // Set mode to Fast PWM with WGM22, WGM21 and WGM20 to high. 
  // Set OC2A and OC2B on Compare mode with COM2A1 and COM2B1 to high
  //ttTCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B |= (1 << WGM22);
  
    
  // No prescaler:
  TCCR2B |=  (1 << CS20);
  // Set CS21 bits for 8 prescaler:
  //TCCR2B |=  (1 << CS21);
  // Set CS20 and CS22 bits for 32 prescaler:
  //TCCR2B |= (1 << CS20) | (1 << CS21);
  
  
  OCR2A=180; 
  //OCR2B=180;
  
  // Set CS20 and CS22 bits for 64 prescaler:
  //TCCR2B |= (1 << CS22);
  // Set CS20 and CS22 bits for 128 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS20);  
  // Set CS20 and CS22 bits for 256 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS21);  
  // Set CS20 and CS22 bits for 1024 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  
  /*

  // Motor 2:
  TCCR1A = 0;
  TCCR1B = 0;

  // Set mode to Fast PWM with WGM22, WGM21 and WGM20 to high. 
  // Set OC2A and OC2B on Compare mode with COM2A1 and COM2B1 to high
  TCCR1A = _BV(WGM11) | _BV(WGM10);
  //  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) ;
  
  
  // No prescaler:
  //TCCR1B |=  (1 << CS10);
  // Set CS20 and CS22 bits for 8 prescaler:
  //TCCR1B |=  (1 << CS11);
  // Set CS20 and CS22 bits for 64 prescaler:
  //TCCR1B |= (1 << CS10) | (1 << CS11);
  // Set CS20 and CS22 bits for 256 prescaler:
  //TCCR1B |= (1 << CS12);  
  // Set CS20 and CS22 bits for 1024 prescaler:
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  
  
  OCR1A=135;   
  
*/
  // enable global interrupts:
  sei(); 
 
  if (verbose)
  {
     Serial.println("[ OK ] Init Timers"); 
  }
}
 
void setup(void) 
{ 
  setupTimers();
  int i;
    pinMode(4, OUTPUT); 
    pinMode(7, OUTPUT); 
    pinMode(11, OUTPUT); 
    pinMode(3, OUTPUT);  
  Serial.begin(19200);      //Set Baud Rate
  Serial.println("Run keyboard control");
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW); 
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  
  stop();
} 
 
void loop(void) 
{
  /*
  static unsigned long timePoint = 0;    // current sense and diagnosis,if you want to use this 
   if(millis() - timePoint > 1000){       //function,please show it & don't forget to connect the IS pins to Arduino                                             
   current_sense();
   timePoint = millis();
   }
   */
  advanceM1(speed1);
  advanceM2(speed2);
  if(Serial.available()){
    char val = Serial.read();
    if(val != -1)
    {
      switch(val)
      {
      case 'w'://Move Forward
        speed1 = 255;
        speed2 = 255;
        advance (255,255);   //move forward in max speed
        break;        
        
      case 's'://Move Backward
        back_off (255,255);   //move back in max speed
        break;
      case 'a'://Turn Left
        turn_L (100,100);        
        break;      
       
      case 'b'://Turn Left
        speed1 = 80;
        speed2 = 80;
        advance(80,80);        
        break;       
      case 'd'://Turn Right
        turn_R (100,100);
        break;
      case 'z':
        Serial.println("Hello");
        break;
      case 'x':
        stop();
        break;
        
        
      case 't':
        speed1 = speed1 + 10;
        Serial.print("M1:    ");
        Serial.println(speed1);
        break;
      case 'g':
        speed1 = speed1 - 10;
        Serial.print("M1:    ");
        Serial.println(speed1);
        break;
        
      case 'p':
        speed2 = speed2 + 10;
        Serial.print("M2:    ");
        Serial.println(speed2);
        break;
      case 'l':
        speed2 = speed2 - 10;
        Serial.print("M2:    ");
        Serial.println(speed2);
        break;
      }
    }
    else stop();  
  }
 
}

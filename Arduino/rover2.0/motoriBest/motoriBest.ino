/**
 **  [Test] We are using timer1 and timer2 in order to change PWM frequency.
 **  Timer 1 :
 **           pin 9  (OC1A) -> leave unplugged
 **           pin 10 (OC1B) -> M1
 **  Timer 2 : 
 **           pin 3  (OC2B) -> M2
 **           pin 11 (OC2A) -> Leave unplugged
 **
 */

int E1 = 9;     //M1 Speed Control
int E2 = 10;     //M2 Speed Control
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

void avanceM1(char a)          //Move forward
{
  analogWrite (E1,a);      
  digitalWrite(M1,HIGH);
} 

void arriereM1(char a)          //Move forward
{
  analogWrite (E1,a);      
  digitalWrite(M1,LOW);
}  

void avanceM2(char b)          //Move forward
{
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  

void arriereM2(char b)          //Move forward
{
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
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

// Timer settings
// Initialize Timer
void setupTimers()
{
  
  cli();
  TCCR1B = 0;
  
  // No prescaler:
  TCCR1B |=  (1 << CS10);
  // Set CS21 bits for 8 prescaler:
  //TCCR2B |=  (1 << CS21);
  // Set CS20 and CS22 bits for 32 prescaler:
  //TCCR2B |= (1 << CS20) | (1 << CS21);
  // Set CS20 and CS22 bits for 64 prescaler:
  //TCCR2B |= (1 << CS22);
  // Set CS20 and CS22 bits for 128 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS20);  
  // Set CS20 and CS22 bits for 256 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS21);  
  // Set CS20 and CS22 bits for 1024 prescaler:
  //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

  // enable global interrupts:
  sei(); 
 
  if (verbose)
  {
     Serial.println("[ OK ] Init PWMs"); 
  }
}
 
void setup(void) 
{ 
  setupTimers();
  int i;
  pinMode(4, OUTPUT); 
  pinMode(7, OUTPUT); 
  pinMode(10, OUTPUT); 
  pinMode(9, OUTPUT);  
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
  if (speed1>=0)
    avanceM1(speed1);
  else
    arriereM1(-speed1);
  if (speed2 >=0)
    avanceM2(speed2);
  else
    arriereM2(-speed2);
  
  
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
       
      case 'y':
        speed1 = speed1 + 2;
        Serial.print("M1:    ");
        Serial.println(speed1);
        break;
      case 'h':
        speed1 = speed1 - 2;
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
        
      case 'o':
        speed2 = speed2 + 2;
        Serial.print("M2:    ");
        Serial.println(speed2);
        break;
      case 'k':
        speed2 = speed2 - 2;
        Serial.print("M2:    ");
        Serial.println(speed2);
        break;
      }
    }
    else stop();  
  }
 
}

#include <Servo.h>
#include <Wire.h>
#include "Propulsion.h"
#include "Ux.h"
#include "PID_v2.h"
#include "NonLinearPid.h"

Ux sakura;
Propulsion tenzoProp(sakura.getM(1),sakura.getM(2),sakura.getM(3),sakura.getM(4));

//NonLinearPid controlCascade(SetpointRoll,SetpointPitch,SetpointYaw,SetpointAltitude);

char readAnswer, readChar, readCh;

// REMOVE

byte modeS;

/**
 * Modes
 */
int connAck = 0;
int takeOff = 0;
int hovering = 0;
int landed = 1;
int tracking = 0;
int warning = 0;

/**
 * VTOL settings
 */
 // Take Off settings
int rampTill = 830;
int idle = 1000;
int motorRampDelayFast = 2;
int motorRampDelayMedium = 5;
int motorRampDelaySlow = 15;
int motorRampDelayVerySlow = 20;

// Safe Mode: after timeToLand ms tenzo will land automatically
unsigned long timeToLand = 20000;
boolean autoLand = false;
boolean landing = false;
int landSpeed = 1;
// Landing protocol variables
unsigned long timerStart;
unsigned long checkpoint;


// Keep track of the state
boolean initializing = false;
boolean initialized = false;

/** 
 ** Control
 **/
 
// W constant (necessary?)
float Kmy = 1, Kw = 3.7;
/**
 * Pid Controller 
 */
 
// Rimesso - check validity
 
boolean autoEnablePid = true;
boolean enablePid = false;

// theta
boolean enableRollPid = true;
boolean enablePitchPid = false;
boolean enableYawPid = false;
// w
boolean enableWRollPid = true;
boolean enableWPitchPid = false;
boolean enableWYawPid = false;
boolean enableAltitudePid = false;
 

// Define IO and setpoint for control
//double SetpointRoll = 0, InputRoll, errorRoll;
// Define the aggressive and conservative Tuning Parameters
        
        // Angle Roll
        float aggKpRoll=0.10, aggKiRoll=0.06, aggKdRoll=0.04;
        float consKpRoll=1.00, consKiRoll=2, consKdRoll=0.03;
        float farKpRoll=0.05, farKiRoll=0.09, farKdRoll=0.03;
        
        // Angle Pitch
        float aggKpPitch=0.07, aggKiPitch=0.06, aggKdPitch=0.04;
        float consKpPitch=2.00, consKiPitch=3.00, consKdPitch=0.00;
        float farKpPitch=0.02, farKiPitch=0.09,  farKdPitch=0.02;
        
        // Angle Yaw
        double aggKpYaw=0.3, aggKiYaw=0.0, aggKdYaw=0.1;
        double consKpYaw=0.3, consKiYaw=0, consKdYaw=0.0;
        
        
        // Altitude  ->> *** Add it, judst created
        double aggKpAltitude=0.2, aggKiAltitude=0.0, aggKdAltitude=0.1;
        double consKpAltitude=0.1, consKiAltitude=0, consKdAltitude=0.1;
        
        ///////////////////////  WWWWWWWWWWWWWWW  //////////////////////
        
        // W Roll
        float aggKpWRoll=0.10, aggKiWRoll=0.06, aggKdWRoll=0.04;
        float consKpWRoll=2.30, consKiWRoll=0.5, consKdWRoll=0.00;
        float farKpWRoll=0.05, farKiWRoll=0.09, farKdWRoll=0.03;
        
        // W Pitch
        float aggKpWPitch=0.07, aggKiWPitch=0.06, aggKdWPitch=0.04;
        float consKpWPitch=3, consKiWPitch=3, consKdWPitch=0.0;
        float farKpWPitch=0.02, farKiWPitch=0.09,  farKdWPitch=0.02;
        
        // W Yaw
        double aggKpWYaw=0.3, aggKiWYaw=0.0, aggKdWYaw=0.1;
        double consKpWYaw=0.3, consKiWYaw=0, consKdWYaw=0.0;
     
        double OutputRoll = 0;
        // Define IO and setpoint for control
        double SetpointRoll = 0, InputRoll, errorRoll;
        
        double SetpointPitch = 0, InputPitch, errorPitch;
        double SetpointYaw = 180, InputYaw, errorYaw;
        double SetpointAltitude = 1, InputAltitude, errorAltitude;
                
        // Define IO and setpoint for control -----------  W
        double SetpointWRoll = 0, InputWRoll = 0, errorWRoll;
        double SetpointWPitch = 0, InputWPitch = 0, errorWPitch;
        double SetpointWYaw = 180, InputWYaw = 0, errorWYaw;
        
        //double OutputRoll = 0;
        double OutputPitch = 0;
        double OutputYaw = 0;
        double OutputAltitude = 0;
        
        
        double OutputWRoll = 0;
        double OutputWPitch = 0;
        double OutputWYaw = 0;



// Threshold
volatile int thresholdRoll = 7;
volatile int thresholdFarRoll = 20;
volatile int thresholdPitch = 7; 
volatile int thresholdFarPitch = 25;
volatile int thresholdYaw = 15;
volatile int thresholdAlt = 20;

// initialize pid outputs
volatile int rollPID = 0;
volatile int pitchPID = 0;
volatile int yawPID = 0;



//Specify the links and initial tuning parameters
PID rollPid(&InputRoll, &OutputRoll, &SetpointRoll, consKpRoll, consKiRoll, consKdRoll, DIRECT);
PID pitchPid(&InputPitch, &OutputPitch, &SetpointPitch, consKpPitch, consKiPitch, consKdPitch, DIRECT);
PID yawPid(&InputYaw, &OutputYaw, &SetpointYaw, consKpYaw, consKiYaw, consKdYaw, DIRECT);
PID altitudePid(&InputAltitude, &OutputAltitude, &SetpointAltitude, consKpAltitude, consKiAltitude, consKdAltitude, DIRECT);


//Specify the links and initial tuning parameters
PID wrollPid(&InputWRoll, &OutputWRoll, &SetpointWRoll, consKpWRoll, consKiWRoll, consKdWRoll, DIRECT);
PID wpitchPid(&InputWPitch, &OutputWPitch, &SetpointWPitch, consKpWPitch, consKiWPitch, consKdWPitch, DIRECT);
PID wyawPid(&InputWYaw, &OutputWYaw, &SetpointWYaw, consKpWYaw, consKiWYaw, consKdWYaw, DIRECT);

/**
 * Compass
 */

// Defines address of CMPS10
#define ADDRESS 0x60      
//CMPS10 my_compass;
float angPosFilter[3];
int pitch1;
int roll1;
int filterAng = 0;


byte highByte, lowByte, fine;    
char pitch, roll;                
int bearing;   

/**
 ** Gyro
 **/

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24


#define STATUS_REG 0x27
#define ZOR_REG 0b01000000
#define ZDA_REG 0b00000100
#define YOR_REG 0b00100000
#define YDA_REG 0b00000010
#define XOR_REG 0b00010000
#define XDA_REG 0b00000001

//use address 104 if CS is not connected
int L3G4200D_Address = 105; 

int zOld, xOld, yOld, xCand, yCand, zCand;
int threshold = 200;

float scale2000 = 70;

float bx= 0,by=0,bz=0;
long bxS,byS,bzS;

unsigned long biasCalcTime = 0;

/**
 ** Acc 
 **/

const float RESOLUTION=800; //0.8 v/g -> resolucion de 1.5g -> 800mV/g
const float VOLTAGE=3.3;  //voltage al que está conectado el acelerómetro

const float ZOUT_1G = 850;   // mv Voltage en Zout a 1G

const int NADJ  = 50;        // Número de lecturas para calcular el error

// Define accelerometer's pins
const int xaxis = 0;
const int yaxis = 1;
const int zaxis = 2;

// 3.3 Fully loaded Tenzo V2.1
//int xRawMin = 425;
//int xRawMax = 280;
//// 
//int yRawMin = 410;
//int yRawMax = 275;
//// 
//int zRawMin = 428;
//int zRawMax = 289;


int xRawMin = 628;
int xRawMax = 404;
 
int yRawMin = 619;
int yRawMax = 398;
 
int zRawMin = 626;
int zRawMax = 426;

float XError,YError,ZError;

// Acc Timers
unsigned long accTimer;
unsigned long lastAccTimer;
unsigned long timeToRead = 0;
unsigned long lastTimeToRead = 0;
unsigned long servoTime = 0;

// delta T control the routine frequency
float deltaT = 1;
float timerLoop = 0, timerReading = 0, timerSec = 0;
float timerRoutine = 0, count = 0;
float redingTime = 0, samplingTime = 0, calcTime =0, printTime = 0;
float k=0, kM1=0, kMReading = 0, kMRoutine=0, kMLoop=0, secRoutine=0;

byte mode;

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

/**
 ** Serial 
 **/
int BaudRateSerial = 115200;
// Bluetooth
int BlueRate = 115200; // Slow down in case
// Gps
int BaudRateGps = 4800;
byte loBytew1, hiBytew1,loBytew2, hiBytew2;
int loWord,hiWord;

int printBlueAngleCounter = 0;
int printBlueAngleSkip = 5;



// Volatile vars
volatile int cont = 0;
int countCtrlAction = 0;
volatile int contSamples=0;
volatile int contCalc=0;

volatile float thetaOLD = 0;
volatile float phi=0;
volatile float theta=0;
volatile float psi=0;
volatile int x=0;
volatile int y = 0;
volatile int z= 0;
volatile int rawAx = 0;
volatile int rawAy = 0;
volatile int rawAz = 0;
volatile int dt=0;

volatile float wF[3];
volatile float aF[3];
volatile boolean filterGyro = false;
volatile boolean filterAcc = false;
volatile boolean initializedSetup = false;
volatile boolean initializedGyroCalib = false;


volatile float bearing1;

volatile float angleXAcc;
volatile float angleYAcc;

volatile float aax,aay,aaz;
volatile float alphaA = 0.2, alphaW = 0.8;
volatile float estXAngle = 0, estYAngle = 0, estZAngle = 0;
volatile float kG = 0.98, kA = 0.02, kGZ=0.60, kAZ = 0.40;



void setupAcceleromter()
{
  pinMode(xaxis,INPUT);
  pinMode(yaxis,INPUT);
  pinMode(zaxis,INPUT);
  
//  #ifdef MMA7820Q
//    XError =  AccelAdjust(xaxis);
//    YError =  AccelAdjust(yaxis);
//    ZError =  AccelAdjust(zaxis);
//    ZError = ZError - ZOUT_1G;
//  #endif
  if (!sakura.getProcessing())
  {
    Serial.println("[OK] Initializing accelerometer");
  }
}

/** 
 ** Servo initialization
 **/
 

void setupGyro()
{
  if (!sakura.getProcessing())
  {
    Serial.println("       Init Gyro");
  }
  
  // Configure L3G4200  - 250, 500 or 2000 deg/sec
  setupL3G4200D(2000); 
  //wait for the sensor to be ready   
  delay(1500); 
  
  biasCalcTime = micros();
  calcBias();
  biasCalcTime = micros() - biasCalcTime;
  
  if (!sakura.getProcessing())
  {
    Serial.print("Readings: [us] ");
    Serial.print(samplingTime);
    Serial.print("      Bias est: [us] ");
    Serial.print(biasCalcTime);
    Serial.print("      Samples: ");
    Serial.println(contSamples);
  }
  if (!sakura.getProcessing())
  {
    Serial.println("[ OK ] Init Gyro");
  }
}

void setupTimerInterrupt()
{
  // Timer settings
  // Initialize Timer
  cli();
  TCCR3A = 0;
  TCCR3B = 0;

  // Set compare match register to the desired timer count
  //OCR3A=77; //16*10^6/(200Hz*1024)-1 = 77 -> 200 Hz 
  OCR3A=193; //16*10^6/(80Hz*1024)-1 = 193 -> 80 Hz 
  //OCR3A=780; //16*10^6/(20Hz*1024)-1 = 780 -> 20 Hz 
  //OCR3A=50; //16*10^6/(308Hz*1024)-1 = 50 -> 308 Hz 

  TCCR3B |= (1 << WGM32);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR3B |= (1 << CS30) | (1 << CS32);
  // enable timer compare interrupt:
  TIMSK3 |= (1 << OCIE3B);

  // enable global interrupts:
  sei(); 
 
  // ADC Tuning
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_32;    // set our own prescaler to 64 
  if (!sakura.getProcessing())
  {
     Serial.println("[ OK ] Init Timers"); 
  }
}

void setupCtx()
{
  initializedSetup = true;
  k = micros();
  if (!sakura.getProcessing())
  {
     Serial.println("[ OK ] Init ctx"); 
  }
}

void setupCommunication()
{
  Wire.begin();
  Serial.begin(115200); 
  Serial.println("[Ok] InitCOM");
}


/// TILL HERE



void setup() {
  setupCommunication();
  
  setupAcceleromter();
  setupGyro();
  setupTimerInterrupt();  
  setupCtx();  
  sakura.welcome();
  tenzoProp.calibrateOnce();
  tenzoProp.init();
  Serial.println("PP");
}

void loop() {  
  SerialRoutine();
  tenzoProp.setSpeedWUs(tenzoProp.getThrottle());
}

void getAcc() //ISR
{
   rawAx=analogRead(xaxis);
   rawAy=analogRead(yaxis);
   rawAz=analogRead(zaxis);
  
  // OLD con MMA7260Q
//   aax = (((rawAx*5000.0)/1023.0)-XError)/RESOLUTION;
//   aay = (((rawAy*5000.0)/1023.0)-YError)/RESOLUTION;
//   aaz = (((rawAz*5000.0)/1023.0)-ZError)/RESOLUTION;

  float xScaled = map(rawAx, xRawMin, xRawMax, -1000, 1000);
  float yScaled = map(rawAy, yRawMin, yRawMax, -1000, 1000);
  float zScaled = map(rawAz, zRawMin, zRawMax, -1000, 1000);
  
  //float zScaled = zm;
  
  // re-scale to fractional Gs
  aax = xScaled / 1000.0;
  aay = yScaled / 1000.0;
  aaz = zScaled / 1000.0;
   
   if (filterAcc)
   {      
      aF[0] = aax;
      aF[1] = aay;
      aF[2] = aaz;
      aFilter(aF);
   }
   // gets the value sample time
   accTimer = micros() - lastAccTimer;
   // updates last reading timer
   lastAccTimer = micros();  
}

void calcAngle() //ISR
{
  // From Gyro
  dt = micros()-k;
  if (!filterGyro)
  {
    phi=phi+(float) x*(float)dt/1000000.0;
    
    thetaOLD = theta; 
    theta=theta+(float) y*(float) dt/1000000.0;

    psi=psi+(float) z*(float) dt/1000000.0; 
  }
  else if (filterGyro)
  {  
    phi=phi+wF[0]*(float)dt/1000000.0;

    thetaOLD = theta;
    theta=theta+wF[1]*(float) dt/1000000.0;

    psi=psi+wF[2]*(float) dt/1000000.0;  
  }
  
  if (filterAcc)
  {
    angleXAcc = -(atan2(-aF[0],-aF[2])) * RAD_TO_DEG;
    angleYAcc = (atan2(-aF[1],-aF[2])) * RAD_TO_DEG;
  }
  else
  {
    // From Acc
    angleXAcc = (atan2(-aax,-aaz)) * RAD_TO_DEG;
    angleYAcc = (atan2(-aay,-aaz)) * RAD_TO_DEG;
  }
  k=micros();  
}

void estAngle() // ISR
{
  estXAngle = (estXAngle + x*(float)dt/1000000.0)*kG + angleXAcc*kA;
  estYAngle = (estYAngle + y*(float)dt/1000000.0)*kG + angleYAcc*kA;
  //estZAngle = (estZAngle + z*(float)dt/1000000.0)*0.02 + bearing1*0.98;
  //psi*KG + yaw*KA;
}

void wFilter(volatile float val[])
{
  val[0] = (1-alphaW)*x + alphaW*val[0];
  val[1] = (1-alphaW)*y + alphaW*val[1];
  val[2] = (1-alphaW)*z + alphaW*val[2];
}

void aFilter(volatile float val[])
{
  val[0] = (1-alphaA)*aax + alphaW*val[0];
  val[1] = (1-alphaA)*aay + alphaW*val[1];
  val[2] = (1-alphaA)*aaz + alphaW*val[2];
}


ISR(TIMER3_COMPB_vect)
{  
  // Mettilo altrimenti non funziona
  sei();
  
  volatile byte statusflag = readRegister(L3G4200D_Address, STATUS_REG);
  while(!(statusflag & ZDA_REG) && (statusflag & ZOR_REG)&&!(statusflag & YDA_REG) && (statusflag & YOR_REG)&& !(statusflag & XDA_REG) && (statusflag & XOR_REG)) 
  {
    statusflag = readRegister(L3G4200D_Address, STATUS_REG);
  }
  //read values
  getGyroValues(); 
  getAcc();
  getCompassValues();
  calcAngle();
  estAngle();
  cont++;
}

void SerialRoutine()
{
  if (Serial.available())
  {
      char t = Serial.read();
      if (t == 't')
        tenzoProp.test();
      else if (t == 'c')
        tenzoProp.calibrateAgain();
      else if (t == 's')
        tenzoProp.stopAll();
      else if (t == 'r')
        tenzoProp.resetMotors();
      else if (t == 'q')
      {
        tenzoProp.setThrottle(tenzoProp.getThrottle() + 10);
        Serial.println(tenzoProp.getThrottle());
      }
      else if (t == 'a')
      {
        tenzoProp.setThrottle(tenzoProp.getThrottle() - 10);
        Serial.println(tenzoProp.getThrottle());
      }
      else if (t == 'v')
      {
        Serial.println(tenzoProp.getThrottle());
      }      
      else if (t == 'x')
      {
        tenzoProp.detachAll();
        
        // To add in method
        resetMotorsPidOff();
      }    
      else if (t == 'i')
      {
        initialize();
      }
      else if (t == 'L')
      {
        land();
      }
      else if (t == 'm')
      {
         sakura.setPrintMotorVals(!sakura.getPrintMotorVals()); 
      }
      else if (t == 'h')
      {
        sakura.setPrintPIDVals(!sakura.getPrintPIDVals());
      }
      else if (t == 'p')
      {
        changePidState(true);
        enablePid = true;
      }
      else if (t == 'l')
      {
        changePidState(false);
        enablePid = false;
      }         
      else if (t == 'b')
      {
        sakura.setSendBlueAngle(!sakura.getSendBlueAngle());
      }         
      else if (t == 'k')
      {
        sakura.setPrintMotorValsUs(!sakura.getPrintMotorValsUs());
      }    
  }
  timerSec = micros()-secRoutine;
  lastTimeToRead = micros();
   
  if (timerSec >= 1000000)
  {
    secRoutine = micros();
    if (!sakura.getProcessing() && sakura.getPrintTimers() && !sakura.getPrintBlue())
    {
      //Serial.print(cont);
      Serial.print("Samples rate: [sample/sec] ");
      Serial.print(contSamples);
      Serial.print("    ControlInput: ");
      Serial.println(countCtrlAction);
      Serial.print("    timeservo: ");
      Serial.println(servoTime);
      Serial.println();
    }
    cont=0;      
    contSamples=0;      
    contCalc=0; 
    countCtrlAction=0;
  }

  timerRoutine = micros()-kMRoutine;
  
  // The following loop runs every 1ms
  if (timerRoutine >= deltaT*1000) 
  {      
    kMRoutine = micros();    
    count += 1;
    if (count >= 1)
    {
      count = 0;
      if (sakura.getSendBlueAngle())
        printSerialAngleNew();
      //control();  
      controlCascade();
      //controlW();
      countCtrlAction++;
      tenzoProp.setSpeeds(tenzoProp.getThrottle(), OutputPitch, OutputRoll, OutputYaw, OutputAltitude);
      //tenzoProp.setSpeeds(tenzoProp.getThrottle(), OutputWPitch, OutputWRoll, OutputWYaw, OutputAltitude);

      //servoTime = micros();
      //servoTime = micros() - servoTime;
      //printAcc();
      //printOmega();
      //printT();
    }
  }
}


void landFast()
{
  for (int j=tenzoProp.getThrottle(); j>40 ;j--)
  {
    tenzoProp.setSpeedWUs(j);
    //Serial.println(j);
    // Kind or brutal land
    delay(motorRampDelayFast);
  }  
  tenzoProp.idle();
}

void getCompassValues()
{
  requestCompassData();
  getCompassData();
  // bearing1 =  my_compass.bearing();
  bearing1 = (float) -(bearing + fine*0.10);
}

void requestCompassData()
{
   //starts communication with CMPS10
   Wire.beginTransmission(ADDRESS); 
   //Sends the register we wish to start reading from   
   Wire.write(2);                 
   Wire.endTransmission();             

   // Request 4 bytes from CMPS10
   Wire.requestFrom(ADDRESS, 4); 
   // Wait for bytes to become available   
   while(Wire.available() < 4);              
   highByte = Wire.read();           
   lowByte = Wire.read();            
   pitch = Wire.read();              
   roll = Wire.read();               
}

void getCompassData()
{
   // Calculate full bearing
   bearing = ((highByte<<8)+lowByte)/10;  

   // Calculate decimal place of bearing   
   fine = ((highByte<<8)+lowByte)%10;   
}


void initializeFast()
{
  if (tenzoProp.getThrottle() == MIN_SIGNAL)
  {
   for (int j=MIN_SIGNAL; j<rampTill;j++)
   {
      tenzoProp.setSpeedWUs(j);
      //Serial.println(j);
      delay(motorRampDelayFast); 
   }
  }
  else if (tenzoProp.getThrottle()<=rampTill)
  {
   for (int j=tenzoProp.getThrottle() ; j<rampTill;j++)
   {
      tenzoProp.setSpeedWUs(j);
      //Serial.println(j);
      delay(motorRampDelayFast); 
   }
  }
  else if (tenzoProp.getThrottle()<=rampTill)
  {
   Serial.println("TODO: initialize FAST called unexpectedly");
  }
  tenzoProp.setThrottle(rampTill);
}


void initialize()
{
  if (!initialized)
  {
    if (!sakura.getProcessing())
        Serial.println("Initializing");
    initializing = true;
    tenzoProp.resetMotors();
    delay(500);
    for (int j=MIN_SIGNAL; j<rampTill;j++)
    {
      tenzoProp.setSpeeds(j, OutputPitch, OutputRoll, OutputYaw, OutputAltitude);
      //motorSpeed(j);
      //if (!processing)
        Serial.println(j);
      delay(motorRampDelayMedium); 
    }
    tenzoProp.setThrottle(rampTill);
    
    if (enablePid)
    {  
      changePidState(true);
    }
    else
    {
      changePidState(false);
    }
    
    checkpoint = millis();
    initialized = true;    
    initializing = false;
  }
  else
  {
    Serial.println();
    Serial.print("First Land ortherwise Tenzo will crash");
    Serial.println();
  }
}


void resetMotorsPidOff()
{
  tenzoProp.setThrottle(MIN_SIGNAL);
  tenzoProp.setSpeedWUs(MIN_SIGNAL);
  // Sets timerStart to 0
  timerStart = 0;
  checkpoint = 0;
  // Disable Pid when motors are off
  if (enablePid)
  {
    // Change only if PID is enabled 
    // Tenzo has landed, no need of controls
    changePidState(false);
  }
} 



void land()
{
  if (initialized)
  {
    landing = true;
    if(!sakura.getProcessing())
    {
      Serial.println();
      Serial.print("Landing protocol started...");
      Serial.print(tenzoProp.getThrottle());
      Serial.print(" ");
    }
    for (int j=tenzoProp.getThrottle(); j>MIN_SIGNAL ;j--)
    //for (int j=throttle; j>0 ;j--)
    {      
      tenzoProp.setSpeeds(j, OutputWPitch, OutputWRoll, OutputWYaw, OutputAltitude);
      //motorSpeed(j);
      Serial.println(j);
      // Kind or brutal land
      if (landSpeed == 1)
        delay(motorRampDelayFast);
      else if (landSpeed == 2)
        delay(motorRampDelayMedium);
      else if (landSpeed == 3)
        delay(motorRampDelaySlow);
      else
        delay(motorRampDelayVerySlow);
    }
    resetMotorsPidOff();
    initialized = false;
    landing = false;;
    // updateStates
    landed=1;
    takeOff=0;
    hovering=0;
  }
  else
  {
    Serial.println();
    Serial.print("Land command received but Tenzo is not Flying   !! WARNING !!");
    Serial.println();
  }
}


void protocol1()
{  
  if (autoLand)
  {
    // If motors are on updates timer
    if (initialized)
      timerStart = millis() - checkpoint;
    // if Tenzo has already been initialized for a timeToLand period, then land
    if (initialized && timerStart>=timeToLand)
    {
      Serial.println("Time to Land my friend!");
      land();
    }
  }
}


void getGyroValues()
{  
  // Get Data if available
  volatile byte statusflag = readRegister(L3G4200D_Address, STATUS_REG);
  while(!(statusflag & ZDA_REG) && (statusflag & ZOR_REG)&&!(statusflag & YDA_REG) && (statusflag & YOR_REG)&& !(statusflag & XDA_REG) && (statusflag & XOR_REG)) 
  {
    statusflag = readRegister(L3G4200D_Address, STATUS_REG);
  }
  
  //starting samplingTimer
  samplingTime = micros();

  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
  
  if (initializedGyroCalib)
    removeBiasAndScale();

  samplingTime = micros()- samplingTime;
  contSamples++;
}


void printOmega()
{
  if (filterGyro)
  {
    Serial.print("       Wx:");
    Serial.print(wF[0]);
  
    Serial.print("       Wy:");
    Serial.print(wF[1]);
  
    Serial.print("       Wz:");
    Serial.print(wF[2]);
  }
  else
  {
    Serial.print("      wx:");
    Serial.print(x);
  
    Serial.print("       wy:");
    Serial.print(y);
  
    Serial.print("       Wz:");
    Serial.println(z);
  }
}

void printAcc()
{
  Serial.println();
  Serial.print(aax);
  Serial.print(",");
  Serial.print(aay);
  Serial.print(",");
  Serial.print(aaz);
  Serial.print(",");
  Serial.println("E");
}

void printSerialAngle()
{
  Serial.print(phi);
  Serial.print(",");
  Serial.print(theta);
  Serial.print(",");
  Serial.print(psi);
  Serial.print(",");
  Serial.print(angleXAcc);
  Serial.print(",");
  Serial.print(angleYAcc);
  Serial.print(",");
  Serial.print(estXAngle);
  Serial.print(",");
  Serial.print(estYAngle);
  Serial.print(",");
  Serial.println(bearing1);
}

void printSerialAngleNew()
{
  Serial.print("P");
  Serial.print(",");
  Serial.print(phi);
  Serial.print(",");
  Serial.print(theta);
  Serial.print(",");
  Serial.print(psi);
  Serial.print(",");
  Serial.print(angleXAcc);
  Serial.print(",");
  Serial.print(angleYAcc);
  Serial.print(",");
  Serial.print(estXAngle);
  Serial.print(",");
  Serial.print(estYAngle);
  Serial.print(",");
  Serial.println(bearing1);
}


void removeBiasAndScale()
{
  x = (x - bx)*scale2000/1000;
  y = (y - by)*scale2000/1000;
  z = (z - bz)*scale2000/1000;
} 

void calcBias()
{
  if (!sakura.getProcessing())
    Serial.println("Decting Bias ...");
  int c = 500;
  for (int i = 0; i<c; i++)
  {
    delay(5);
    getGyroValues(); 
    bxS = bxS + x;
    byS = byS + y;
    bzS = bzS + z;
    //Serial.println(i);
  }

  bx = bxS / c;
  by = byS / c;
  bz = bzS / c;

  if (!sakura.getProcessing())
  {
    Serial.println(bx);
    Serial.println(by);
    Serial.println(bz);
  }  
  initializedGyroCalib = true;
}

void writeRegister(int deviceAddress, byte address, byte val) 
{
  Wire.beginTransmission(deviceAddress); // start transmission to device 
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address)
{
  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while(!Wire.available()) 
  {
    // waiting
    // Serial.println("No Data");
  }

  v = Wire.read();
  return v;
}

int setupL3G4200D(int scale)
{
  //From  Jim Lindblom of Sparkfun's code

    // Enable x, y, z and turn off power down:
  //writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);
  // 400Hz and 1Hz cutoff frq of the HPF
  //writeRegister(L3G4200D_Address, CTRL_REG1, 0b01011111);
  // 200Hz 
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b01001111);

  // If you'd like to adjust/use the HPF:
  // High pass filter cut off frecuency configuration
  // ODR 200Hz, cutoff 0.02Hz 1001
  //writeRegister(L3G4200D_Address, CTRL_REG2, 0b00001001);
  // ODR 200Hz, cutoff 1 Hz 0100
  //writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000100);
  // ODR 200Hz, cutoff 0.02Hz 1001
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00001001);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  //writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250)
  {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }
  else if(scale == 500)
  {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }
  else
  {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00010000);
  //writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}


void control()
{
  if (enablePid)
  {
    // Roll PID
    if (enableRollPid)
    {
      //Serial.println("    ZAK ");
      InputRoll = estXAngle;
      //Serial.println("    ZAK ");
      errorRoll = abs(SetpointRoll - estXAngle); //distance away from setpoint
      
        rollPid.SetTunings(consKpRoll, consKiRoll, consKdRoll);
        
      rollPid.Compute(); // Computes outputRoll

      if (sakura.getPrintPIDVals())
      {
        Serial.println();
        Serial.print("INPUT ");
        Serial.print(InputRoll);
        Serial.print("ErrorRoll:");
        Serial.print(errorRoll);
        Serial.print("Roll PID: ");
        Serial.print(OutputRoll);
        Serial.println();
      }
    }
    else
    {
      Serial.println();
      Serial.println("Roll SS");
      Serial.println();
      OutputRoll = 0;
    }

    // Pitch PID1
    if (enablePitchPid)
    {
      InputPitch = estYAngle;
      errorPitch = abs(SetpointPitch - estYAngle); //distance away from setpoint
      pitchPid.SetTunings(consKpPitch, consKiPitch, consKdPitch);
      pitchPid.Compute(); // Computes outputPitch

      if (sakura.getPrintPIDVals())
      { 
        Serial.print("E:  ");
        Serial.print(errorPitch);
        Serial.print(" A:");
        Serial.print(OutputPitch);
        Serial.println();
      }
    }  
    else
    {
      Serial.println();
      Serial.println("SS Pitch");
      Serial.println();
      OutputPitch = 0;
    }

    if (sakura.getPrintPIDVals())
    {
      Serial.println();      
    }
  }
  else
  {
    OutputRoll = 0;
    OutputPitch = 0;    
    OutputYaw = 0;
  }
}

void controlCascade()
{
  // Roll PID
    if (enableRollPid)
    {
      //Serial.println("    ZAK ");
      InputRoll = estXAngle;
      //Serial.println("    ZAK ");
      errorRoll = abs(SetpointRoll - estXAngle); //distance away from setpoint
      
        rollPid.SetTunings(consKpRoll, consKiRoll, consKdRoll);
        
      rollPid.Compute(); // Computes outputRoll

      //if (sakura.getPrintPIDVals())
      {
        Serial.println();
        Serial.print("INPUT ");
        Serial.print(InputRoll);
        Serial.print("ErrorRoll:");
        Serial.print(errorRoll);
        Serial.print("Roll PID: ");
        Serial.print(OutputRoll);
        Serial.println();
      }
      
      InputWRoll = x;
      errorWRoll = abs(OutputRoll - x); 

      wrollPid.SetTunings(Kw*consKpWRoll, Kw*consKiWRoll, Kw*consKdWRoll);

      wrollPid.Compute();
    }
    else
    {
      Serial.println();
      Serial.println("Warning");
      Serial.println();
      OutputRoll = 0;
    }

}

void controlW()
{
  //Serial.println("K1");
  if (enablePid)
  {
    //Serial.println("K2");
    // Roll W PID
    if (enableWRollPid)
    {
      InputWRoll = x;
      errorWRoll = abs(SetpointWRoll - x); 

      wrollPid.SetTunings(Kw*consKpWRoll, Kw*consKiWRoll, Kw*consKdWRoll);

      wrollPid.Compute();
    }
    else
    {
      OutputWRoll = 0;
    }

    // Pitch PID1
    if (enableWPitchPid)
    {
      InputWPitch = y;
      errorWPitch = abs(SetpointWPitch - y);

      wpitchPid.SetTunings(Kw*consKpWPitch, Kw*consKiWPitch, Kw*consKdWPitch);

      wpitchPid.Compute(); // Computes outputPitch
    }  
    else
    {
      OutputWPitch = 0;
    }

    /*
    // Yaw PID
     if (enableWYawPid)
     {
     if (filterAng == 1)
     {
     InputWYaw = z;
     errorWYaw = abs(SetpointWYaw - z); //distance away from setpoint
     }
     
     if(errorYaw<thresholdYaw)
     {
     //we're close to setpoint, use conservative tuning parameters
     wyawPid.SetTunings(Kw*consKpWYaw, Kw*consKiWYaw, Kw*consKdWYaw);
     }   
     wyawPid.Compute(); 
     
     }
     else
     {
     OutputYaw=0;
     }
     */
    //if (printPIDVals)
    //{
      //Serial.println();      
    //}
  }
  else
  {
    OutputWRoll = 0;
    OutputWPitch = 0;    
    OutputWYaw = 0;
  }
}

void changePidState(boolean cond)
{
  if (cond)
  {
    // Enable Pid Actions
    rollPid.SetMode(AUTOMATIC);
    //tell the PID to range between 0 and the full throttle
    //SetpointRoll = 0;
    rollPid.SetOutputLimits(-500, 500);

    // Pitch
    pitchPid.SetMode(AUTOMATIC);
    //SetpointPitch = 0;
    //tell the PID to range between 0 and the full throttle
    pitchPid.SetOutputLimits(-500, 500);

    // Yaw
    yawPid.SetMode(AUTOMATIC);
    //SetpointYaw=0;
    //tell the PID to range between 0 and the full throttle
    yawPid.SetOutputLimits(-500, 500);

    // Enable Pid Actions
    wrollPid.SetMode(AUTOMATIC);
    //tell the PID to range between 0 and the full throttle
    //SetpointRoll = 0;
    wrollPid.SetOutputLimits(-500, 500);

    // Pitch
    wpitchPid.SetMode(AUTOMATIC);
    //SetpointPitch = 0;
    //tell the PID to range between 0 and the full throttle
    wpitchPid.SetOutputLimits(-500, 500);

    // Yaw
    wyawPid.SetMode(AUTOMATIC);
    //SetpointYaw=0;
    //tell the PID to range between 0 and the full throttle
    wyawPid.SetOutputLimits(-500, 500);

    enablePid = true;
  }
  else
  { 
    rollPid.SetMode(MANUAL);
    pitchPid.SetMode(MANUAL);
    yawPid.SetMode(MANUAL);
    wrollPid.SetMode(MANUAL);
    wpitchPid.SetMode(MANUAL);
    wyawPid.SetMode(MANUAL);
    
    
    enablePid = false;
  }
 
}

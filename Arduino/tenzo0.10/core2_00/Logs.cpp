/*
  Propulsion.cpp - Library for Sd Card Logging code.
  Created by Giovanni Balestrieri - UserK, June 29, 2015
  Released into the public domain.
*/

#include <Arduino.h>
#include "Logs.h"
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

Logs::Logs()
{
  extern RTC_DS1307 RTC;
  
  File logFile = SD.open(logPath, FILE_WRITE);
  logFile.close();
  File warningFile = SD.open(warningPath, FILE_WRITE);
  warningFile.close();
  File errorFile = SD.open(errorPath, FILE_WRITE);
  errorFile.close();
  File wcetFile = SD.open(wcetPath, FILE_WRITE);
  wcetFile.close();
}

boolean Logs::init()
{
  Serial.print("\nInitializing SD card...");
  pinMode(53, OUTPUT);
   
  if (!SD.begin(_chipSelect)) 
  {
    Serial.println("SdCard failed. Continuing without log:");
    cardOK = false;
  } 
  else 
  {
    Serial.println("Wiring is correct and a card is present.");
    cardOK = true;
  }  
  return cardOK;
}

/*
 * Log states
 */
void Logs::logStates(int initialized, int hovering, int landed)
{
  if (this->cardOK && this->checkFilesExistence)
  {
    this->openLogFile();
    logFile.print("S,");
    logFile.print(initialized);
    logFile.print(',');
    logFile.print(hovering);
    logFile.print(',');
    logFile.print(landed);
    logFile.println();
    this->closeLogFile(); 
  }
}

/*
 * Log Accelerometer's value
 */
void logAcc(float x,float y,float z)
{
  if (this->cardOK && this->checkFilesExistence)
  {
    this->openLogFile();
    logFile.print("Acc,");
    logFile.print(x);
    logFile.print(',');
    logFile.print(y);
    logFile.print(',');
    logFile.print(z);
    logFile.print(",z");
    logFile.println();
    this->closeLogFile(); 
  }
}

/*
 * Log Gyro's value
 */
void logGyro(float x,float y,float z)
{
  if (this->cardOK && this->checkFilesExistence)
  {
    this->openLogFile();
    logFile.print("Gyro,");
    logFile.print(x);
    logFile.print(',');
    logFile.print(y);
    logFile.print(',');
    logFile.print(z);
    logFile.print(",z");
    logFile.println();
    this->closeLogFile(); 
  }
}

/*
 * Log Session Data
 */
void Logs::logSession()
{
  if (this->cardOK && this->checkFilesExistence)
  {
    now_instant = RTC.now();

    this->openLogFile();
    
    logFile.print(" Session ");
    logFile.print(now_instant.day());
    logFile.print('/');
    //We print the month
    logFile.print(now_instant.month());
    logFile.print('/');
    //We print the year
    logFile.print(now_instant.year());
    logFile.print(' ');
    //We print the hour
    logFile.print(now_instant.hour());
    logFile.print(':');
    //We print the minutes
    logFile.print(now_instant.minute());
    logFile.print(':');
    //We print the seconds
    logFile.print(now_instant.second());
    logFile.println();
    
    this->closeLogFile();  
  }  
}

/*
 * Checks File existence
 */
boolean Logs::checkFileLog()
{
  if (SD.exists(logPath))
  {
    logFileCheck = true;
    logFile.close();
  }  
  else 
  {
    logFileCheck = false;    
  }
  return logFileCheck;
}

/*
 * Checks File existence
 */
boolean Logs::checkFileError()
{
  if (SD.exists(errorPath))
  {
    errorFileCheck = true;
    errorFile.close();
  }  
  else 
  {
    errorFileCheck = false;    
  }
  return errorFileCheck;
}


/*
 * Checks File existence
 */
boolean Logs::checkFileWarning()
{
  if (SD.exists(warningPath))
  {
    warningFileCheck = true;
    warningFile.close();
  }  
  else 
  {
    warningFileCheck = false;    
  }
  return warningFileCheck;
}

/*
 * Checks File existence
 */
boolean Logs::checkFileWcet()
{
  if (SD.exists(wcetPath))
  {
    wcetFileCheck = true;
    wcetFile.close();
  }  
  else 
  {
    wcetFileCheck = false;    
  }
  return wcetFileCheck;
}

/*
 * Checks All Files
 */
boolean Logs::checkFiles()
{
  if (this->checkFileWcet() && this->checkFileWarning() && this->checkFileError() && this->checkFileLog())
  {
    checkFilesExistence = true;
  }
  else
  {
    checkFilesExistence = false;
  }
  
  return checkFilesExistence;
}

void Logs::openLogFile()
{
  logFile = SD.open(logPath,FILE_WRITE);
}

void Logs::closeLogFile()
{
  logFile.close();
}

void Logs::openErrorFile()
{
  errorFile = SD.open(errorPath,FILE_WRITE);
}

void Logs::closeErrorFile()
{
  errorFile.close();
}

void Logs::openWarningFile()
{
  warningFile = SD.open(warningPath,FILE_WRITE);
}

void Logs::closeWarningFile()
{
  warningFile.close();
}

void Logs::openWcetFile()
{
  wcetFile = SD.open(wcetPath,FILE_WRITE);
}

void Logs::closeWcetFile()
{
  wcetFile.close();
}


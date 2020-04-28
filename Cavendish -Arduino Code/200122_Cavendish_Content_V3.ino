int speedvariable = 10; // speed of light up 0-255
int overlapspeed = 1; // overlap speed of lights
// MASTER  DUE
//SLAVE 01 -SOUND TOUCHBOARD - CONNECTED TO SDA,SCL
//SLAVE 02- 2ND DUE LIGHTS WITH SHIELD - CONNECTED TO  SDA1 SCL1




#include "buttonTrigger.h"
#include <Wire.h>
#include <SPI.h>
#include "lights01.h"
#include "sensor.h"
#include "AnamServo.h"

// servo timer
bool timerDone = false;

//=====================SEQUENCER=============================================================
int x = 0; // for iterating through the array of sequences
int checkState = 0; // check where you are in the sequence
int internalSequenceState = 0; // checking internal timers run out to begin main sequences (states)

int RoutineA [5] = {30, 35, 33, 36, 36}; 
int RoutineB [5] = {30, 35, 33, 36, 36};
int RoutineC [5] = {30, 35, 33, 36, 36}; 


int lenRoutine1 = 5; // length of the routine - 4 sequences long so 4 state changes

int randomSeqPicker = 0;
bool sequenceChosen = false; //when the sequence is chosen return true then false when it is over
bool endofSequence = false; // when the sequence is over returns true
int choiceState = 0;

//=======================================================================


// AnamServo(String _servoName, int _relayPin, int _dataPin, int _servoSpeed, int _startPos, int _firstPos, int _secondPos)
AnamServo leftServo("left 5 FLOOR SIDE", 1, 5, 6, 1, 1650, 850, 1650); //5,6 (actual servo) //12,42
AnamServo rightServo("right 4 FLOOR SIDE", 2, 7, 8, 1, 1125, 1850, 1125);   //7/8(actual servo) //5,6


// light colour value
int r = 255;
int g = 77;
int b = 0;
int w = 32;

// LIGHTS
//Lights(int _numberofPixels, int _dataPinLight, int _rVal, int _gVal, int _bVal, int _wVal, int _pixelfadeSpeed)
Lights basementToTower(22, 26, r, g, b, w, 1); //26 (actual pin) //7

// leds inside plinth
//===============================Light Variables=======================================================================
int ledPin = 11;    // LED connected to digital pin 9
int fadeValue = 0;


//===== Settings variables===========================================================================
int state = 0;          // keeps track of what part of the sequence is currently running


long timer = 0;         // use this to store times for timers

int buttonWait = 5;     // enables the sensor for 5 seconds after button was pressed
int initialWait = 2;    // wait 2 seconds in the beginning after trigger before opening the model
int waitTime = 10;       // time in seconds to wait in between opening and closing sequence for test
unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
bool buttonPushed = false;


int sensorState = 0;

//==================================MODEL IDLE STATE===================================
int idleState = 0;
const unsigned long noButtonPushSampleIntervalMs = 1000;

unsigned long longCountMax = 600; //500*25 = 12500= 12.5 seconds 600x1000 = 10 minutes

int longIdleCount = 0;

unsigned long idlePreviousMillis = 0;

long idleStartTime = 0;

void resetCounter()  // set counter to 0
{
  Serial.println("button not pushed start counter");
  longIdleCount = 0;
}
//===============================================================================================
void stateMover(String msg, long t, int s, int cS)
{
  Serial.println(msg);
  timer = t;
  state = s;
  checkState = cS;
}

inline void wireTransfer(byte b)
{
    Wire.beginTransmission(20);
    Wire.write(b);
    Wire.endTransmission();
}

void setup()
{
  Wire.begin();
  fadeValue = 0; // start with no light
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
  Serial.begin(19200);
  buttonTriggerSetup();
  leftServo.setup();
  rightServo.setup();
  basementToTower.setup();
  randomSeed(analogRead(0));
  Serial.println("Ready...");
  Serial.println("Ready to Reset");
  int i = 12;
  for (i = 12; i < 17; i = i + 1)  // fade ON SPINE
  {
    basementToTower.fadeUpStart(i, 1);
    Serial.println("fade up spine");
  }
  bool lightsFadeUpDone = basementToTower.lightSequence();
  if (lightsFadeUpDone == true)
  {
    Serial.println("done lights on");
  }


}

void loop()
{
  if (state == 0 && sequenceChosen == false && choiceState == 0 && x == 0)
  {
    //SEQUENCE SELECTION
    /*if (Serial.available() > 1)
      {
      //read the incoming byte:
      randomSeqPicker = Serial.parseInt();
      if (randomSeqPicker < 1) return;  // because it doesn't fucking want to pass an int
      sequenceChosen = true;
      Serial.println("sequence chosen");
      choiceState = 1;
      // say what you got:
      Serial.print("I received: ");
      Serial.println(randomSeqPicker);*/

     //MANUAL STATE SELECTION
    /*if (Serial.available() > 1)
     {
      int tempValue = Serial.parseInt();
      if (tempValue < 1) return;  // because it doesn't fucking want to pass an int
      sequenceChosen = true;
      randomSeqPicker = 1;
      choiceState = 1;
      RoutineA[0] = tempValue;
      Serial.print("I received: ");
      Serial.println(tempValue); 
    }*/


    if (buttonTriggerSensor() == true)
    {
      buttonPushed = true;
      Serial.println("button pushed");
    }
    if ( buttonPushed == true)

    {
      //if timer has run out, then do the following...
      buttonPushed = false;
      Serial.println("button pushed");
      Serial.print("random picked: ");
      randomSeqPicker = random(1, 3); // CHOOSE A ROUTINE RANDOMLY
      analogWrite(ledPin, 255);
      Serial.println(randomSeqPicker);
      sequenceChosen = true;
      Serial.println("sequence chosen");
      choiceState = 1;
    }
  }

  if (choiceState == 1)
  {
    if (x >= lenRoutine1 && checkState == 0) // if the last seq number is complete then it is the end of seq reset everything
    {
      Serial.println("repick a sequence");
      endofSequence = true;
      sequenceChosen = false;
      choiceState = 0;
      state = 0;
      x = 0; // reset x
      Serial.println("resetting");
      //reset the arduino
      //__DSB;
      //SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);//software reset
      //RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
      //NVIC_SystemReset();
    }
    if (randomSeqPicker == 1 && choiceState == 1) // TRIGGER ROUTINE A
    {

      if (x <= lenRoutine1 && checkState == 0)
      {
        Serial.println("Trigger routine A");
        stateMover("move to state " + String(RoutineA[x]) + "!", millis(), RoutineA[x], 1);
      }
    }
    else if (randomSeqPicker == 2 && choiceState == 1) // TRIGGER ROUTINE B
    {
      if (x <= lenRoutine1 && checkState == 0)
      {
        Serial.println("Trigger routine B");
        stateMover("move to state " + String(RoutineB[x]) + "!", millis(), RoutineB[x], 1);
      }
    }
    else if (randomSeqPicker == 3 && choiceState == 1 ) // TRIGGER ROUTINE C
    {
      if (x <= lenRoutine1 && checkState == 0)
      {
        Serial.println("Trigger routine C");
        stateMover("move to state " + String(RoutineC[x]) + "!", millis(), RoutineC[x], 1);
      }
    }

    //======================================== Dead STATE =============================================================
    if (state == 20)
    {
      Serial.println("dead state");
      Serial.println("turn plinth lights on");
      analogWrite(ledPin, 5);
      x++; // get the next index value for the array
      state = 0;
      checkState = 0;
      internalSequenceState = 0;
    }

    else if (state == 36)
    {
      Serial.println("dead state");
      Serial.println("turn plinth lights off");
      analogWrite(ledPin, 0);
      wireTransfer(2); // send sound to stop
      x++; // get the next index value for the array
      state = 0;
      checkState = 0;
      internalSequenceState = 0;
    }


    // ==========================ALL FADE UP SPINE=============
    else if (state == 30)
    {
      if (internalSequenceState == 0 )
      {
        leftServo.openStart();

        wireTransfer(1); // send sound to start
        
        int i = 12;
        for (i = 12; i < 17; i = i + 1)  // fade ON SPINE
        {
          basementToTower.fadeUpStart(i, 1);
          internalSequenceState = 1;
          timer = millis();
          Serial.println("fade up basement to tower");
        }

      }

      if (internalSequenceState == 1 )
      {
        bool servoSlideDone;
        bool servoVertDone;
        servoVertDone = leftServo.callUpdate();


        if (millis() >= timer + 2 * 1000 && timerDone == false )
        {
          rightServo.openStart();
          timerDone = true; // tell me when the n seconds is up
          Serial.println("timer done begin right side open");
        }

        if (timerDone == true)
        {
          servoSlideDone = rightServo.callUpdate();
        }
        //Serial.print("time after timer:  ");
        //Serial.println(millis() - timer);
        //Serial.println("start left servo moving"); //BASEMENT ON
        if (millis() >= timer + (overlapspeed * 5) * 1000 )
        {
          int i = 0;
          for (i = 0; i < 5; i = i + 1)
          {

            basementToTower.fadeUpStart(i, speedvariable);
          }
        }

        if (millis() >= timer + (overlapspeed * 15) * 1000 )
        {
          basementToTower.fadeUpStart(5, speedvariable);
          basementToTower.fadeUpStart(6, speedvariable);
          //Serial.println("start ground lights up");
        }
        if (millis() >= timer + (overlapspeed * 16) * 1000 )
        {
          basementToTower.fadeUpStart(7, speedvariable);

          basementToTower.fadeUpStart(17, speedvariable);
          //Serial.println("start first lights up");
        }
        if (millis() >= timer + (overlapspeed * 17) * 1000 )
        {
          basementToTower.fadeUpStart(8, speedvariable);

          basementToTower.fadeUpStart(18, speedvariable);
          //Serial.println("start second lights up");
        }
        if (millis() >= timer + (overlapspeed * 18) * 1000 )
        {
          basementToTower.fadeUpStart(9, speedvariable);

          basementToTower.fadeUpStart(19, speedvariable);
          //Serial.println("start third lights up");
        }
        if (millis() >= timer + (overlapspeed * 19) * 1000 )
        {
          basementToTower.fadeUpStart(10, speedvariable);

          basementToTower.fadeUpStart(20, speedvariable);
          //Serial.println("start fourth lights up");
        }
        if (millis() >= timer + (overlapspeed * 20) * 1000 )
        {
          basementToTower.fadeUpStart(11, speedvariable);

          basementToTower.fadeUpStart(21, speedvariable);
          //Serial.println("start fifth lights up");
        }

        bool lightsFadeUpDone = basementToTower.lightSequence();
        if (servoVertDone == true && servoSlideDone == true && lightsFadeUpDone == true)
        {
          Serial.println("model open");
          x++; // get the next index value for the array
          state = 0;
          checkState = 0;
          internalSequenceState = 0;
          timerDone = false;
        }
      }
    }


    // ==========================CLOSE CAVENDISH=============
    else if (state == 33)
    {
      if (internalSequenceState == 0 )
      {
        leftServo.closeStart();
        rightServo.closeStart();

        internalSequenceState = 1;
        timer = millis();



      }

      if (internalSequenceState == 1 )
      {
        bool servoSlideDone;
        bool servoVertDone;
        servoVertDone = leftServo.callUpdate();
        servoSlideDone = rightServo.callUpdate();

        //Serial.print("time after timer:  ");
        //Serial.println(millis() - timer);
        //Serial.println("start left servo moving"); //BASEMENT ON
        if (millis() >= timer + (overlapspeed * 7) * 1000 )
        {
          int i = 0;
          for (i = 0; i < 5; i = i + 1)
          {

            basementToTower.fadeDownStart(i, speedvariable);
          }
        }

        if (millis() >= timer + (overlapspeed * 8) * 1000 )
        {
          basementToTower.fadeDownStart(5, speedvariable);
          basementToTower.fadeDownStart(6, speedvariable);
          //Serial.println("start ground lights up");
        }
        if (millis() >= timer + (overlapspeed * 9) * 1000 )
        {
          basementToTower.fadeDownStart(7, speedvariable);

          basementToTower.fadeDownStart(17, speedvariable);
          //Serial.println("start first lights up");
        }
        if (millis() >= timer + (overlapspeed * 10) * 1000 )
        {
          basementToTower.fadeDownStart(8, speedvariable);

          basementToTower.fadeDownStart(18, speedvariable);
          //Serial.println("start second lights up");
        }
        if (millis() >= timer + (overlapspeed * 11) * 1000 )
        {
          basementToTower.fadeDownStart(9, speedvariable);

          basementToTower.fadeDownStart(19, speedvariable);
          //Serial.println("start third lights up");
        }
        if (millis() >= timer + (overlapspeed * 12) * 1000 )
        {
          basementToTower.fadeDownStart(10, speedvariable);

          basementToTower.fadeDownStart(20, speedvariable);
          //Serial.println("start fourth lights up");
        }
        if (millis() >= timer + (overlapspeed * 13) * 1000 )
        {
          basementToTower.fadeDownStart(11, speedvariable);

          basementToTower.fadeDownStart(21, speedvariable);
          //Serial.println("start fifth lights up");
        }


        bool lightsFadeUpDone = basementToTower.lightSequence();
        if (servoVertDone == true && servoSlideDone == true && lightsFadeUpDone == true)
        {
          Serial.println("model closed lights up!");

          timerDone = false;

          internalSequenceState = 2;

        }
      }
      if (internalSequenceState == 2)
      {
        bool lightsFadeUpDone = basementToTower.lightSequence();
        if (lightsFadeUpDone == true)
        {
          Serial.println("turn plinth lights off");
          analogWrite(ledPin, 0);
          Serial.println("end sequence");
          x++; // get the next index value for the array
          state = 0;
          checkState = 0;
          internalSequenceState = 0;
        }
      }
    }


 


    //==========================================================9 SECOND DEAD PAUSE ==========================================================================================================
    else if (state == 35)
    {
      if (internalSequenceState == 0 && millis() >= timer + 9 * 1000)
      {
        Serial.println("9 second pause over");
        x++; // get the next index value for the array
        state = 0;
        checkState = 0;
        internalSequenceState = 0;
      }

    }

    //=============================ALL LIGHTS FADE ON ======================================================
    else if (state == 37)
    {
      int i = 0;
      for (i = 0; i < 22; i = i + 1)  // fade ON SPINE
      {
        basementToTower.fadeUpStart(i, 1);
        internalSequenceState = 1;
        Serial.println("fade up ALL");
      }
      if (internalSequenceState == 1)
      {
        bool lightsFadeUpDone = basementToTower.lightSequence();
        if (lightsFadeUpDone == true)
        {
          Serial.println("all on!");
          x++; // get the next index value for the array
          state = 0;
          checkState = 0;
          internalSequenceState = 0;
        }
      }
    }

    //=============================ALL LIGHTS FADE off ======================================================
    else if (state == 38)
    {
      int i = 0;
      for (i = 0; i < 22; i = i + 1)  // fade ON SPINE
      {
        basementToTower.fadeDownStart(i, 1);
        internalSequenceState = 1;
        Serial.println("fade down ALL");
      }
      if (internalSequenceState == 1)
      {
        bool lightsfadeDownDone = basementToTower.lightSequence();
        if (lightsfadeDownDone == true)
        {
          Serial.println("all off!");
          x++; // get the next index value for the array
          state = 0;
          checkState = 0;
          internalSequenceState = 0;
        }
      }
    }

  }
}

#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
// sd card instantiation
SdFat sd;
 //SOFTWARE RESET ARDUINO
void(* resetFunc) (void) = 0; //declare reset function @ address 0
// ==========================================church song stuff==============================================================================
#define cavendishSound 0 // bird content
int durationTrackhotel = 347;  //duration of bird sound
long hotelstartTime;  // start millis() for sequence 
bool hotelplaying = false;
// restart the time for the bird sequence
void SoundHotelStart() 
{
  hotelstartTime = millis();
}

long SoundHoutelMillis() 
{
  return millis() - hotelstartTime;
}

//=========================================== main setup ===================================================================================



void setup() 
{
  Wire.begin(20);                // join i2c bus with address 
  Wire.onReceive(receiveEvent); // register event from Due to Touchboard receive trigger data
  Serial.begin(115200);           // start serial for output
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt(); // sound stuff 
  MP3player.begin();   
  MP3player.setVolume(10,10); // volume control for later fade this up and down 
  hotelstartTime = 0; // reset the start time

}

//===================================main trigger loop for sounds ==============================================================
void loop() 
{

}

//receive data from the due, to trigger the mp3 files
void receiveEvent(int howMany)   // special event function for wire 
{
  while (1 < Wire.available()) 
  { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.print("Received Trigger: ");
  Serial.println(x);         // print the integer
  
  // trigger church song from Due==================================================================
  if (x == 1)
  {
    MP3player.playTrack(cavendishSound);
  }
  
  //stop church song from Due
  if (x == 2)
  {
    MP3player.stopTrack();
  }



}

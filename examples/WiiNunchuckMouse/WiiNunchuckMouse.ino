#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

#include <WiiChuck.h>
#include <Mouse.h>
#include <EEPROM.h>

Accessory nunchuck1;

//Write to EEPROM for Global Access

bool contCZ = false;

uint8_t trigZ = 0;
uint8_t trigC = 0;


void setup() {
  Serial.begin(115200);
  nunchuck1.begin();

  if (nunchuck1.type == Unknown) {
    /** If the device isn't auto-detected, set the type explicatly
     *  NUNCHUCK,
     WIICLASSIC,
     GuitarHeroController,
     GuitarHeroWorldTourDrums,
     DrumController,
     DrawsomeTablet,
     Turntable
     */
    nunchuck1.type = NUNCHUCK;
  }
}

void loop() {
  
  /*For debugging
  nunchuck1.printInputs(); //For printing all values
  Serial.println(nunchuck1.values[0]); //joyX
  Serial.println(nunchuck1.values[1]); //joyY
  Serial.println(nunchuck1.values[10]); //Z button (0 or 255)
  Serial.println(nunchuck1.values[11]); //C button (0 or 255)
  */
  
  nunchuck1.readData();    // Read inputs and update maps
 
//Mouse Move
  uint8_t joystickX = nunchuck1.values[0];
  uint8_t joystickY = nunchuck1.values[1];

  uint8_t deltaX = 0;
  uint8_t deltay = 0;
  deltaX = (joystickX - 127)/4;
  deltay = (128 - joystickY)/4;

  Mouse.move(deltaX, deltay, 0);

//Mouse Click Setup
  bool buttZ = false;
  bool buttC = false;

  if (nunchuck1.values[10] == 255)
      buttZ = true;
  if (nunchuck1.values[11] == 255)
      buttC = true;
      
 //Mouse Click Code
  if (buttZ && trigZ == 0)
     trigZ += 1; //Write to EEPROM
     if (buttZ && trigZ == 1)
        Mouse.press(MOUSE_RIGHT);
        trigZ += 1; //Write to EEPROM
     if (buttZ == false && trigZ == 1)
        Mouse.click(MOUSE_RIGHT);
        trigZ == 0; //Reset EEPROM
      if (buttZ == false && trigZ == 2)
        Mouse.release(MOUSE_RIGHT);
        trigZ == 0; //Reset EEPROM

  if (buttC && trigC == 0)
     trigC += 1; //Write to EEPROM
     if (buttC && trigC == 1)
        Mouse.press(MOUSE_LEFT);
        trigC += 1; //Write to EEPROM
     if (buttC == false && trigC == 1)
        Mouse.click(MOUSE_LEFT);
        trigC == 0; //Reset EEPROM
      if (buttC == false && trigC == 2)
        Mouse.release(MOUSE_LEFT);
        trigC == 0; //Reset EEPROM
     
//Middle Button code
  if (buttC && buttZ)
    if (contCZ)
      Mouse.release(MOUSE_MIDDLE);
    else
      Mouse.press(MOUSE_MIDDLE);
      contCZ == true; //Write to EEPROM


//Sensitivity
delay(10);
      
}

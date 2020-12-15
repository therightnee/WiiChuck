#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

#include <WiiChuck.h>
#include <Mouse.h>

Accessory nunchuck1;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 60; // the debounce time; increase if the output flickers

unsigned long holdTime = 90;
unsigned long switchPressZ = 0;
unsigned long switchPressC = 0;
unsigned long switchPressCZ = 0;


//Press State Variables
int trigZ = 0;
int trigC = 0;
int trigCZ = 0;

uint8_t deltaX = 0;
uint8_t deltay = 0;
int scroll = 0;

//Write to EEPROM for Global Access
void setup() {
  Serial.begin(115200);
  nunchuck1.begin();
  lastDebounceTime = millis(); 
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
  Mouse.begin();
//Mouse Move
  uint8_t joystickX = nunchuck1.values[0];
  uint8_t joystickY = nunchuck1.values[1];
  uint8_t accelX = nunchuck1.values[4];
  
//Mouse Click Variables
  uint8_t liveX = nunchuck1.values[10];
  uint8_t liveY = nunchuck1.values[11];
  bool buttZ = false;
  bool buttC = false;
//Convert Press to Bool
  if (liveX == 255){
      buttZ = true;
  }
  if (liveY == 255){
      buttC = true;
  }
      
  deltaX = (joystickX - 127)/7;
  deltay = (128 - joystickY)/7;
  scroll = 0;
  //Serial.println(accelX);
  
  if (accelX < 120 && buttZ && buttC == false) {
    scroll = (accelX - 90)/10;
    //Serial.println(scroll);
    trigZ = 3;
  }

  Mouse.move(deltaX, deltay, scroll);


//Mouse Click Code

if ((millis() - lastDebounceTime) > debounceDelay) {

//Right (button Z)
  if (buttZ && buttC == false){
     if (trigZ == 0){
        trigZ = 1;
        switchPressZ = millis();
        //Serial.println("Trigger Right");  
     }    
     if (trigZ == 1){
        if ((millis() - switchPressZ) > holdTime) {
            Mouse.press(MOUSE_RIGHT);
            trigZ = 2;
            //Serial.println("Trigger Right Press");
        }
     }  
  }
     
//Left (button C)
  if (buttC && buttZ == false){
     if (trigC == 0){
        trigC = 1;
        switchPressC = millis();
        //Serial.println("Trigger Left");
     }
     if (trigC == 1){
        if ((millis() - switchPressC) > holdTime) {
            Mouse.press(MOUSE_LEFT);
            trigC = 2;
        }
     }
  }
        
//Middle (button C&Z)
  if (buttC && buttZ){
    if (trigCZ == 0){
        trigCZ = 1;
        switchPressCZ = millis();
    }
    if (trigCZ == 1){
        if ((millis() - switchPressCZ) > holdTime) {
            Mouse.press(MOUSE_MIDDLE);
            trigCZ = 2;
        }
     }
  }

  if (buttZ == false && buttC == false){
    //Right Click
    if (trigZ == 1){
        Mouse.click(MOUSE_RIGHT);
        trigZ = 0;
    }  
    if (trigZ == 2){
        Mouse.release(MOUSE_RIGHT);
        trigZ = 0;
        //Serial.println("Right Press Release");
    }
    if (trigZ == 3){
      trigZ = 0;
    }
    
    //Left Click
    if (trigC == 1){
        Mouse.click(MOUSE_LEFT);
        trigC = 0;
    }
    if (trigC == 2){
        Mouse.release(MOUSE_LEFT);
        trigC = 0;
    }

    //Middle Click
    if (trigCZ == 1){
        Mouse.click(MOUSE_MIDDLE);
        trigCZ = 0;
    }
    if (trigCZ == 2){
        Mouse.release(MOUSE_MIDDLE);
        trigCZ = 0;
    }

    
  }

  lastDebounceTime = millis(); 
}


    
//Sensitivity
delay(10);  
}

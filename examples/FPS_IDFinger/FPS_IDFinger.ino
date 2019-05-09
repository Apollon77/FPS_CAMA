/*
	FPS_Enroll.ino - Library example for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, August 02rd 2017

	This sketch runs the identification procedure and returns the detected template id.
*/

#include "FPS_CAMA.h"
/*
// Can also be used with SoftwareSerial
#include "SoftwareSerial.h"

// Hardware setup - FPS connected to:
//	  digital pin 4(arduino rx, fps tx)
//	  digital pin 5(arduino tx - 560ohm resistor fps tx - 1000ohm resistor - ground)
//		this brings the 5v tx line down to about 3.2v so we don't fry our fps

SoftwareSerial fpsSoftSerial(4, 5); // RX, TX

FPS_CAMA fps(fpsSoftSerial);

// but we use an Arduino Mega here, so we use Serial1 directly on pins 19 (RX) and 18 (TX)
*/
FPS_CAMA fps(Serial1);

/* This method is always called when a user action is required and is working
 * like a callback.
 *
 * The method will get the following parameter to identify the current action and step:
 * command: Current command, enumeration Command_Packet::Commands::Commands_Enum
 * action:  Current required user action: enumeration Response_Packet::ResultCodes::ResultCodes_Enum
 *
 * return value is boolean
 * - true if everything is ok and process should be continued
 * - false if process should be canceled (FPS will get a FPCancel command from
     library and initial call will return in an FPCancel Error)
*/
bool FPSUserAction(const Command_Packet::Commands::Commands_Enum command, const Response_Packet::ResultCodes::ResultCodes_Enum action)
{
  Serial.println("User Action Required:");
  Serial.println();
  Serial.print("Command=0x");
  Serial.print(command,HEX);
  Serial.print(", action=0x");
  Serial.println(action,HEX);
  if (command == Command_Packet::Commands::Identify) {
      Serial.println();
      switch(action) {
        case Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER: // Verify, Identify, Enroll
          Serial.println("Scan done. Please release your Finger from the scanning area.");
          break;
        default:
          Serial.println(" -- UNKNOWN ACTION!!!! -- ");
          return false; // we cancel the process
      }
      Serial.println();
  }
  else {
      Serial.println("Unexpected command ...");
      return false;  // we cancel the process
  }
  return true;
}

void setup()
{
	  Serial.begin(9600);

    Serial1.begin(115200); // start with communication to Device
	  //fpsSoftSerial.begin(115200);
    fps.setUserActionCallback(FPSUserAction); // set callback method, see above
    fps.setSerialTimeout(20000);
    fps.setDebug(true); // so you can see the messages in the serial debug screen

    bool testSuccess = fps.testConnection(true);
    Serial.print("FPS - testConnection = ");
    Serial.println(testSuccess);
    Serial.println();
    Serial.println();

    word val = fps.getSecurityLevel();
    Serial.print("FPS - getSecurityLevel = ");
    Serial.println(val);
    Serial.println();
    Serial.println();

    val = fps.getFingerTimeout();
    Serial.print("FPS - getFingerTimeout = ");
    Serial.println(val);
    Serial.println();
    Serial.println();

    val = fps.getDeviceId();
    Serial.print("FPS - getDeviceId = ");
    Serial.println(val);
    Serial.println();
    Serial.println();

    val = fps.getFirmwareVersion();
    Serial.print("FPS - getFirmwareVersion = ");
    Serial.println(val, HEX);
    Serial.println();
    Serial.println();

    val = fps.getEnrollCount();
    Serial.print("FPS - getEnrollCount = ");
    Serial.println(val);
    Serial.println();
    Serial.println();

    bool val2 = fps.getDuplicationCheck();
    Serial.print("FPS - getDuplicationCheck = ");
    Serial.println(val2);
    Serial.println();
    Serial.println();

    val = fps.getBrokenTemplate();
    Serial.print("FPS - getBrokenTemplate = ");
    Serial.println(val);
    Serial.println();
    Serial.println();

    Serial.println("Please put your finger on the scanning area to start the identification ...");
    Serial.println();

    fps.setSensorLed(true); // turn on the LED inside the fps
}

void loop()
{
	// Identify fingerprint test
    if (fps.fingerDetect())
	{
		word detectedTemplateId = fps.identify();
		if (detectedTemplateId == 0)
		{
            Serial.print("Finger not detected or ERROR while identification: getLastResultCode() =");
            Serial.println(fps.getLastResultCode());
		}
		else
		{
            Serial.print("Finger Successfully detected: Id=");
			Serial.println(detectedTemplateId);
 
		}

        Serial.println();
        Serial.println();
        Serial.println("Please put your finger on the scanning area to start the identification ...");
        Serial.println();
        fps.setSensorLed(true); // turn on the LED inside the fps
	}
	delay(1000);

}

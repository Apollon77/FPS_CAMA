/*
	FPS_Enroll.ino - Library example for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, August 02rd 2017

	This simple sketch outputs some basic settings at the start and then turns the
    LED on and off similar to the Arduino blink sketch.
	It is used to show that communications are working.
*/

#include "FPS_CAMA.h"
/*
// Can also be used with SoftwareSerial
#include "SoftwareSerial.h"

// Hardware setup - FPS connected to:
//	  digital pin 4(arduino rx, fps tx)
//	  digital pin 5(arduino tx - 560ohm resistor fps tx - 1000ohm resistor - ground)
//		this brings the 5v tx line down to about 3.2v so we dont fry our fps

SoftwareSerial fpsSoftSerial(4, 5); // RX, TX

FPS_CAMA fps(fpsSoftSerial);

// but we use an Arduino Mega here, so we use Serial1 directly on pins 19 (RX) and 18 (TX)
*/
FPS_CAMA fps(Serial1);

void setup()
{
   	Serial.begin(9600);
    Serial1.begin(115200); // start with communication to Device
	  //fpsSoftSerial.begin(115200);
    fps.setSerialTimeout(6000);
	  fps.useSerialDebug = true; // so you can see the messages in the serial debug screen

    bool testSuccess = fps.testConnection(true);
    Serial.print("FPS - testConnection = ");
    Serial.println(testSuccess);

    word val = fps.getSecurityLevel();
    Serial.print("FPS - getSecurityLevel = ");
    Serial.println(val);

    val = fps.getFingerTimeout();
    Serial.print("FPS - getFingerTimeout = ");
    Serial.println(val);

    val = fps.getDeviceId();
    Serial.print("FPS - getDeviceId = ");
    Serial.println(val);

    val = fps.getFirmwareVersion();
    Serial.print("FPS - getFirmwareVersion = ");
    Serial.println(val, HEX);

    val = fps.getEnrollCount();
    Serial.print("FPS - getEnrollCount = ");
    Serial.println(val);

    bool val2 = fps.getDuplicationCheck();
    Serial.print("FPS - getDuplicationCheck = ");
    Serial.println(val2);
}


void loop()
{
	// FPS Blink LED Test
	fps.setSensorLed(true); // turn on the LED inside the fps
	delay(1000);
	fps.setSensorLed(false);// turn off the LED inside the fps
	delay(1000);
}

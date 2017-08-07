/*
	FPS_CAMA.h v1.0 - Library for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, December 2015
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
	TLDR; Wil Wheaton's Law
*/

#include "FPS_CAMA.h"

//DONE
Command_Packet::Command_Packet(Commands::Commands_Enum _command, word _dataLength) :
  command(_command), dataLength(_dataLength)
{
	memset(commandData, 0, sizeof(commandData)); // Clear Command Data Array
};

// returns the 24 bytes of the generated command packet
// remember to call delete on the returned array
//DONE
byte* Command_Packet::getPacketBytes()
{
	byte* packetbytes= new byte[24];

	// update command before calculating checksum (important!)
	word cmd = command;

	packetbytes[0] = COMMAND_START_CODE_1;
	packetbytes[1] = COMMAND_START_CODE_2;
	packetbytes[2] = getLowByte(cmd);
	packetbytes[3] = getHighByte(cmd);
	packetbytes[4] = getLowByte(dataLength);
	packetbytes[5] = getHighByte(dataLength);
	for (byte i=0;i<16;i++) packetbytes[i+6] = commandData[i];

	word checksum = calculateChecksum(packetbytes);

	packetbytes[22] = getLowByte(checksum);
	packetbytes[23] = getHighByte(checksum);

	return packetbytes;
}

// Returns the high byte from a word
//DONE
byte Command_Packet::getHighByte(word w)
{
	return (byte)(w>>8)&0x00FF;
}

// Returns the low byte from a word
//DONE
byte Command_Packet::getLowByte(word w)
{
	return (byte)w&0x00FF;
}

// add a byte to CommandData array starting at Index index
//DONE
void Command_Packet::addByteToCommandData(byte index, byte value)
{
	commandData[index] = value;
}

// add a word to CommandData array starting at Index index
//DONE
void Command_Packet::addWordToCommandData(byte index, word value)
{
	commandData[index] = getLowByte(value);
	commandData[index+1] = getHighByte(value);
}

// add an int to CommandData array starting at Index index
//DONE
void Command_Packet::addIntToCommandData(byte index, int value)
{
	commandData[index] = (value & 0x000000ff);
	commandData[index+1] = (value & 0x0000ff00) >> 8;
	commandData[index+2] = (value & 0x00ff0000) >> 16;
	commandData[index+3] = (value & 0xff000000) >> 24;
}

// calculate Checksum
//DONE
word Command_Packet::calculateChecksum(byte* data)
{
	word w = 0;
	for (byte i=0;i<22;i++) {
	  w += data[i];
	}

	return w;
}



// creates and parses a response packet from the finger print scanner
//DONE
Response_Packet::Response_Packet(byte* buffer, word awaitedResponseCode, bool useSerialDebug)
{
    memset(dataBytes, 0, sizeof(dataBytes)); // Clear DataBytes Array
    if (buffer != NULL) {
        validResponse = true;
    	validResponse &= checkParsing(buffer[0], RESPONSE_START_CODE_1, RESPONSE_START_CODE_1, (char *)"RESPONSE_START_CODE_1", useSerialDebug);
    	validResponse &= checkParsing(buffer[1], RESPONSE_START_CODE_2, RESPONSE_START_CODE_2, (char *)"RESPONSE_START_CODE_2", useSerialDebug);

    	word checksum = calculateChecksum(buffer, 22);
    	byte checksum_low = getLowByte(checksum);
    	byte checksum_high = getHighByte(checksum);
    	validResponse &= checkParsing(buffer[22], checksum_low, checksum_low, (char *)"Checksum_LOW", useSerialDebug);
    	validResponse &= checkParsing(buffer[23], checksum_high, checksum_high, (char *)"Checksum_HIGH", useSerialDebug);

        if (awaitedResponseCode != 0xFFFF) {
            byte responseCode_low=getLowByte(awaitedResponseCode);
        	byte responseCode_high=getHighByte(awaitedResponseCode);
        	validResponse &= checkParsing(buffer[2], responseCode_low, responseCode_low, (char *)"ResponseCode_HIGH", useSerialDebug);
        	validResponse &= checkParsing(buffer[3], responseCode_high, responseCode_high, (char *)"ResponseCode_LOW", useSerialDebug);
        }
    	dataLength=wordFromBytes(buffer,4)-2;

    	resultCode = Response_Packet::ResultCodes::parseFromBytes(wordFromBytes(buffer,6));
    	if (resultCode==0) isError=false;
    	  else if (resultCode==1) isError=true;
    	  else validResponse=false;

    	if (dataLength>0) {
    		for (word i=0; i < dataLength; i++)
    		{
    			dataBytes[i]=buffer[8+i];
    		}
    	}
    }
    else {
        validResponse = false;

    }
}

// parses bytes into one of the possible errors from the finger print scanner
//DONE
Response_Packet::ResultCodes::ResultCodes_Enum Response_Packet::ResultCodes::parseFromBytes(word resultCode)
{
	ResultCodes_Enum e = INVALID;
	switch(resultCode)
	{
		case 0x00: 		e = ERR_SUCCESS; break;
		case 0x01: 		e = ERR_FAIL; break;
		case 0x11: 		e = ERR_VERIFY; break;
		case 0x12: 		e = ERR_IDENTIFY; break;
		case 0x13: 		e = ERR_TMPL_EMPTY; break;
		case 0x14: 		e = ERR_TMPL_NOT_EMPTY; break;
		case 0x15: 		e = ERR_ALL_TMPL_EMPTY; break;
		case 0x16: 		e = ERR_EMPTY_ID_NOEXIST; break;
		case 0x17: 		e = ERR_BROKEN_ID_NOEXIST; break;
		case 0x18: 		e = ERR_INVALID_TMPL_DATA; break;
		case 0x19: 		e = ERR_DUPLICATION_ID; break;
		case 0x21: 		e = ERR_BAD_QUALITY; break;
		case 0x23: 		e = ERR_TIME_OUT; break;
		case 0x24: 		e = ERR_NOT_AUTHORIZED; break;
		case 0x30: 		e = ERR_GENERALIZE; break;
		case 0x41: 		e = ERR_FP_CANCEL; break;
		case 0x50: 		e = ERR_INTERNAL; break;
		case 0x51: 		e = ERR_MEMORY; break;
		case 0x52: 		e = ERR_EXCEPTION; break;
		case 0x60: 		e = ERR_INVALID_TMPL_NO; break;
		case 0x61: 		e = ERR_INVALID_SEC_VAL; break;
		case 0x62: 		e = ERR_INVALID_TIME_OUT; break;
		case 0x63: 		e = ERR_INVALID_BAUDRATE; break;
		case 0x64: 		e = ERR_DEVICE_ID_EMPTY; break;
		case 0x65: 		e = ERR_INVALID_DUP_VAL; break;
		case 0x70: 		e = ERR_INVALID_PARAM; break;
		case 0x71: 		e = ERR_NO_RELEASE; break;
		case 0xA1: 		e = GD_DOWNLOAD_SUCCESS; break;
		case 0xFFF1: 	e = GD_NEED_FIRST_SWEEP; break;
		case 0xFFF2: 	e = GD_NEED_SECOND_SWEEP; break;
		case 0xFFF3: 	e = GD_NEED_THIRD_SWEEP; break;
		case 0xFFF4: 	e = GD_NEED_RELEASE_FINGER; break;
		//case 0x01: 	e = GD_DETECT_FINGER; break;
		//case 0x00: 	e = GD_NO_DETECT_FINGER; break;
		//case 0x01: 	e = GD_TEMPLATE_NOT_EMPTY; break;
		//case 0x00: 	e = GD_TEMPLATE_EMPTY; break;
	}
	return e;
}

// Gets an int from the parameter bytes
//DONE
int Response_Packet::intFromBytes(byte* buffer, byte index)
{
	int retval = 0;
	retval = (retval << 8) + buffer[index+3];
	retval = (retval << 8) + buffer[index+2];
	retval = (retval << 8) + buffer[index+1];
	retval = (retval << 8) + buffer[index+0];
	return retval;
}

// Gets an int from the parameter bytes
//DONE
word Response_Packet::wordFromBytes(byte* buffer, byte index)
{
	word retval = 0;
	retval = (retval << 8) + buffer[index+1];
	retval = (retval << 8) + buffer[index+0];
	return retval;
}

// calculates the checksum from the bytes in the packet
//DONE
word Response_Packet::calculateChecksum(byte* buffer, int length)
{
	word checksum = 0;
	for (int i=0; i<length; i++)
	{
		checksum +=buffer[i];
	}
	return checksum;
}

// Returns the high byte from a word
//DONE
byte Response_Packet::getHighByte(word w)
{
	return (byte)(w>>8)&0x00FF;
}

// Returns the low byte from a word
//DONE
byte Response_Packet::getLowByte(word w)
{
	return (byte)w&0x00FF;
}

// checks to see if the byte is the proper value, and logs it to the serial channel if not
//DONE
bool Response_Packet::checkParsing(byte b, byte propervalue, byte alternatevalue, char* varname, bool useSerialDebug)
{
	bool retval = (b != propervalue) && (b != alternatevalue);
	if ((useSerialDebug) && (retval))
	{
		Serial.print("Response_Packet parsing error ");
		Serial.print(varname);
		Serial.print(" ");
		Serial.print(propervalue, HEX);
		Serial.print(" || ");
		Serial.print(alternatevalue, HEX);
		Serial.print(" != ");
		Serial.println(b, HEX);
	}
	return (! retval);
}


//#pragma region -= Data_Packet =-
//void Data_Packet::StartNewPacket()
//{
//	Data_Packet::NextPacketID = 0;
//	Data_Packet::CheckSum = 0;
//}
//#pragma endregion


// Creates a new object to interface with the fingerprint scanner
FPS_CAMA::FPS_CAMA(Stream& streamInstance)
	: _serial(&streamInstance)
{
  // nothing to do
};

// SetSerialTimeout
void FPS_CAMA::setSerialTimeout(word timeout)
{
  serialTimeout = timeout;
};
/*
  [Function]
     One to one match

  [Parameter]
  	 templateId - ID of template to verify the scanned finger with

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the templateNo. is invalid, result=ERR_INVALID_TMPL_NO;
	2) if the templateNo. there is no templatedata, result=ERR_TMPL_EMPTY;
	3) detect finger whether press on sensor or not repeatedly,
	   if no any finger press on sensor in the period of timeout, result= ERR_TIME_OUT;
	4) else check quality of the captured fingerprint image
	   if image is not good, result= ERR_BAD_QUALITY
	5) else result=GD_NEED_RELEASE_FINGER, denote that lift finger;
	6) verify the captured fingerprint with the appoint templatedata
	   if verify ok ,result=Template No.; else result= ERR_VERIFY;
	7) In the period of process verify command, if module received “FPCancel” command,
	   The module then stop verify command and return ACK that is ERR_FP_CANCEL;
*/
bool FPS_CAMA::verify(word templateId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - verify");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::Verify,2);
	cp->addWordToCommandData(0, templateId);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp1 = getResponse(Command_Packet::Commands::Verify);

	lastResultCode = 0xFFFF;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::Verify, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	sendFPCancel();
				    }
				}
				Response_Packet* rp2 = getResponse(Command_Packet::Commands::Verify);
				lastResultCode = 0xFFFF;
				if (rp2->validResponse) {
					if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
					if ((! rp2->isError) && (rp2->dataLength > 0)) {
						if (rp2->wordFromBytes(rp2->dataBytes,0) == templateId) res=true;
						  else lastResultCode=Response_Packet::ResultCodes::ERR_FAIL;
					}
                    else lastResultCode = rp2->wordFromBytes(rp2->dataBytes,0);
                    if (cancel && (lastResultCode == Response_Packet::ResultCodes::ERR_FP_CANCEL)) {
                        getFPCancelResult();
                    }
				}
				delete rp2;
			}
		}
	}
	delete rp1;

	if (useSerialDebug) {
		Serial.print("FPS - verify Result for templateId ");
        Serial.print(templateId);
        Serial.print("=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     One to many match
     The live scanning fingerprint matches to all templates stored in FLASH memory, and then
     respond the result.

  [Parameter]
  	 None

  [Return value]
  	 matched template No. or 0 on error

  [Operation Sequence]
	1) if Template DataBase is null,result=ERR_ALL_TMPL_EMPTY;
	2) detect finger whether press on sensor or not repeatedly;
	   if no any finger press on sensor in the period of timeout, result= ERR_TIME_OUT;
	3) else check quality of the captured fingerprint image,
	if image is not good, result= ERR_BAD_QUALITY
	4) else result=GD_NEED_RELEASE_FINGER, denote that lift finger
	5) identify the captured fingerprint with the all templatedata in FLASH memory,
	   if identify ok ,result=matched template No.; else result= ERR_IDENTIFY
	6) In the period of process identify command, if module received FPCancel command,
	   The module then stop identify command and return ACK that is ERR_FP_CANCEL
*/
word FPS_CAMA::identify()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - identify");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::Identify,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp1 = getResponse(Command_Packet::Commands::Identify);

	lastResultCode = 0xFFFF;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::Identify, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	sendFPCancel();
				    }
				}
				Response_Packet* rp2 = getResponse(Command_Packet::Commands::Identify);
				lastResultCode = 0xFFFF;
				if (rp2->validResponse) {
					if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
					if ((! rp2->isError) && (rp2->dataLength > 0)) {
						res=rp2->wordFromBytes(rp2->dataBytes,0);
					}
                    else lastResultCode = rp2->wordFromBytes(rp2->dataBytes,0);
                    if (cancel && (lastResultCode == Response_Packet::ResultCodes::ERR_FP_CANCEL)) {
                        getFPCancelResult();
                    }
				}
				delete rp2;
			}
		}
	}
	delete rp1;

	if (useSerialDebug) {
		Serial.print("FPS - identify Result templateId=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Enroll
     In the process of enroll, The same finger must be press on the sensor for 3 times,
     each time module get template temporary and storage in RAM, if the three templates
     are correct, the module generalize the three templates to one template then write
     to Flash memory.

  [Parameter]
  	 templateId - ID of template to store the scanned finger in

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the appoint templateNo. is invalid, result=ERR_INVALID_TMPL_NO;
    2) if the templateNo. have existed template data, result=ERR_TMPL_NOT_EMPTY;
    3) else result=GD_NEED_FIRST_SWEEP, denote that press finger for the first time;
    4) detect finger whether press on sensor or not repeatedly,
       if no finger press on sensor in the period of timeout,result=ERR_TIME_OUT;
    5) else check quality of captured finger image,
       if image is no good,result=ERR_BAD_QUALITY
    6) else result=GD_NEED_RELEASE_FINGER denote that lift finger,
       after finger release then goto next step;
    7) result=GD_NEED_SECOND_SWEEP denote that press the same finger for the second time,
       and then goto steep 4,5
    8) if the second input fingerprint is correct,
       result=GD_NEED_RELEASE_FINGER denote that lift finger,
       after finger release then goto next steep;
    9) result=GD_NEED_THIRD_SWEEPdenotethatpressthesamefingerforthethirdtime
       and then goto step 4,5
   10) if the third input fingerprint is correct,
       result=GD_NEED_RELEASE_FINGER denote that lift finger;
   11) the module generalize the three templates to one template,
       success goto step 12, fail result=ERR_GENERALIZE;
   12) if Duplication Check=OFF, storage the template data and return result=Template No;
   13) if Duplication Check=ON, the template data match to all template to check whether
       exist duplicated fingerprint or not. if yes, result = ERR_DUPLICATION_ID;
       else result = Template No. and sorage the template data;
   14) In the period of process ENROLL command,if module received FP Cancel command,
       the module then stop ENROLL command and return ACK that is ERR_FP_CANCEL;
*/
bool FPS_CAMA::enroll(word templateId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - enroll");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::Enroll,2);
	cp->addWordToCommandData(0, templateId);


	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

    bool awaitReleaseFinger=false;
    bool awaitFinalResponse=false;
    bool cancel=false;
	while (true) {
		Response_Packet* rp = getResponse(Command_Packet::Commands::Enroll);

		lastResultCode = 0xFFFF;
		if (rp->validResponse) {
			if (rp->dataLength > 0) lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
			if (! rp->isError) {
				if ((awaitFinalResponse) && (lastResultCode != Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) {
					if (rp->dataLength > 0) lastResultCode = rp->resultCode;
					if ((! rp->isError) && (rp->dataLength > 0)) {
						if (rp->wordFromBytes(rp->dataBytes,0) == templateId) res=true;
							else lastResultCode=Response_Packet::ResultCodes::ERR_VERIFY;
					}
                    else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
					break;
				}
				else if ((lastResultCode == Response_Packet::ResultCodes::GD_NEED_FIRST_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_SECOND_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_THIRD_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) {
					if (((awaitReleaseFinger) && (lastResultCode != Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) ||
					    ((! awaitReleaseFinger) && (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER))) {
					  // ERROR in Process
					  sendFPCancel();
                      cancel=true;
					  break;
					}

					cancel=false;
					if (userActionCallback) {
					    cancel=(! userActionCallback(Command_Packet::Commands::Enroll, Response_Packet::ResultCodes::parseFromBytes(lastResultCode)));
					    if (cancel) {
					    	sendFPCancel();
					    	break;
				    	}
				    }
				    if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_THIRD_SWEEP) {
				    	awaitFinalResponse=true;
				    }
				    awaitReleaseFinger=(lastResultCode != Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER);
				}
				else {
					// ERROR in Process
					sendFPCancel();
                    cancel=true;
					break;
				}
			}
			else {
				break;
			}
		}
        else {
            break;
        }
		delete rp;
	}
    if (cancel) {
        Response_Packet* rp = getResponse(Command_Packet::Commands::Enroll);

		lastResultCode = 0xFFFF;
		if (rp->validResponse) {
			if (rp->dataLength > 0) lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
        }
        if (cancel && (lastResultCode == Response_Packet::ResultCodes::ERR_FP_CANCEL)) {
            getFPCancelResult();
        }
    }

	if (useSerialDebug) {
		Serial.print("FPS - enroll Result for templateId ");
        Serial.print(templateId);
        Serial.print("=");
        Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Enroll One Time
     Enroll One Time command require the finger press on sensor for one time only

  [Parameter]
  	 templateId - ID of template to store the scanned finger in

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the appoint templateNo. is invalid, result=ERR_INVALID_TMPL_NO;
    2) if the templateNo. have existed template data, result=ERR_TMPL_NOT_EMPTY;
    3) detect finger whether press on sensor or not repeatedly,
       if no finger press on sensor in the period of timeout,result=ERR_TIME_OUT;
    4) else check quality of captured finger image,
       if image is no good,result=ERR_BAD_QUALITY
    5) else result=GD_NEED_RELEASE_FINGER denote that lift finger,
    6) if Duplication Check=OFF, storage the template data and return result=Template No;
    7) if Duplication Check=ON, the template data match to all template to check whether
       exist duplicated fingerprint or not. if yes, result = ERR_DUPLICATION_ID;
       else result = Template No. and sorage the template data;
    8) In the period of process ENROLL One Time command,if module received FP Cancel command,
       the module then stop ENROLL One Time command and return ACK that is ERR_FP_CANCEL;
    NOTE: we recommend using Enroll command,disapproval using Enroll one time command
*/
bool FPS_CAMA::enrollOneTime(word templateId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - enrollOneTime");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::EnrollOneTime,2);
	cp->addWordToCommandData(0, templateId);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp1 = getResponse(Command_Packet::Commands::EnrollOneTime);

	lastResultCode = 0xFFFF;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::EnrollOneTime, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	sendFPCancel();
				    }
				}
				Response_Packet* rp2 = getResponse(Command_Packet::Commands::EnrollOneTime);
				lastResultCode = 0xFFFF;
				if (rp2->validResponse) {
					if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
					if ((! rp2->isError) && (rp2->dataLength > 0)) {
						if (rp2->wordFromBytes(rp2->dataBytes,0) == templateId) res=true;
						  else lastResultCode=Response_Packet::ResultCodes::ERR_FAIL;
					}
                    else lastResultCode = rp2->wordFromBytes(rp2->dataBytes,0);
				}
				delete rp2;
                if (cancel && (lastResultCode == Response_Packet::ResultCodes::ERR_FP_CANCEL)) {
                    getFPCancelResult();
                }
			}
		}
	}
	delete rp1;

	if (useSerialDebug) {
		Serial.print("FPS - enrollOneTime Result for templateId ");
        Serial.print(templateId);
        Serial.print("=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Clear Template
     Delete fingerprint data with specified ID from database. After this command is
     executed, fingerprint data with specified ID are deleted immediately.

  [Parameter]
  	 templateId - ID of template to clear

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the appoint template No. is invalid, result= ERR_INVALID_TMPL_NO
	2) if the appoint template No. is inexistence template data, result=ERR_TMPL_EMPTY
	3) else delete specified template data and then return response packet
*/
bool FPS_CAMA::clearTemplate(word templateId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - clearTemplate");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::ClearTemplate,2);
	cp->addWordToCommandData(0, templateId);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::ClearTemplate);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
			if (rp->wordFromBytes(rp->dataBytes,0) == templateId) res=true;
				else lastResultCode=Response_Packet::ResultCodes::ERR_FAIL;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - clearTemplate Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Clear All Template
     Delete all fingerprint data in database. After this command is executed, all
     fingerprint data in database are deleted immediately.

  [Parameter]
  	 None

  [Return value]
  	 number of deleted templates or 0 on error
*/
word FPS_CAMA::clearAllTemplate()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - clearAllTemplate");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::ClearAllTemplate,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::ClearAllTemplate);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) res =  rp->wordFromBytes(rp->dataBytes,0);
            else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - clearAllTemplate Result number deleted=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Empty ID
     Get the first template No. it can be used for storage fingerprint template

  [Parameter]
  	 None

  [Return value]
  	 first free template number or 0 on error

  [Operation Sequence]
	Search the first number that can be used for storage fingerprint template, if all
	fingerprint data base is full,result= ERR_EMPTY_ID_NOEXIST;
	else result=template No. that can be utilized
*/
word FPS_CAMA::getEmptyId()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getEmptyId");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetEmptyId,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetEmptyId);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) res =  rp->wordFromBytes(rp->dataBytes,0);
    		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getEmptyId Result first free=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Template Status
     Check the specified Template No. whether can be utilized or not.
  [Parameter]
  	 templateId - ID of template to check

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the appoint template No. is invalid, result= ERR_INVALID_TMPL_NO
    2) if the appoint template No. is used, result=GD_TEMPLATE_NOT_EMPTY
    3) if the appoint template No. is free, result=GD_TEMPLATE_EMPTY
*/
bool FPS_CAMA::getTemplateStatus(word templateId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - getTemplateStatus");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetTemplateStatus,2);
	cp->addWordToCommandData(0, templateId);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetTemplateStatus);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
            if (lastResultCode == Response_Packet::ResultCodes::GD_TEMPLATE_EMPTY) res=true;
		}
        else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getTemplateStatus Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Broken Template
     Check fingerprint template Data base whether is damage or not.some unit of FLASH memory may
     be damaged by chance failure
     For the broken template data, you can delete the template and then enroll again.

  [Parameter]
  	 None

  [Return value]
  	 first broken template number or 0 on ok

  [Operation Sequence]
	If exist broken template data,result=total number of broken template + template No.
    else total number=0,Template No.=0
*/
word FPS_CAMA::getBrokenTemplate()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getBrokenTemplate");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetBrokenTemplate,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetBrokenTemplate);

	lastResultCode = 0xFFFF;
    word brokenCount = 0;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength >= 4)) {
            brokenCount = rp->wordFromBytes(rp->dataBytes,0);
            if (brokenCount > 0) res =  rp->wordFromBytes(rp->dataBytes,2);
        }
        else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getBrokenTemplate Result first broken=");
		Serial.print(res);
        Serial.print(", number Broken=");
		Serial.print(brokenCount);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Set Security Level
     Set up the threshold of fingerprint identification engine, Integer of 1-5 can be selected,
     one is the lowest identification level and five is the highest identification level,
     Default is three
  [Parameter]
  	 securityLevel - Level value 1..5

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if value of the security level is invalid,result=ERR_INVALID_SEC_VAL
    2) else update the value of Security Level,then response the command
*/
bool FPS_CAMA::setSecurityLevel(word securityLevel)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setSecurityLevel");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SetSecurityLevel,2);
	cp->addWordToCommandData(0, securityLevel);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SetSecurityLevel);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            if (rp->wordFromBytes(rp->dataBytes,0) == securityLevel) res = true;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setSecurityLevel Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Security Level
     Host read the value of Security Level from module

  [Parameter]
  	 None

  [Return value]
  	 security Level value 1..5

  [Operation Sequence]

*/
word FPS_CAMA::getSecurityLevel()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getSecurityLevel");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetSecurityLevel,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetSecurityLevel);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res =  rp->wordFromBytes(rp->dataBytes,0);
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getSecurityLevel Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Set Finger Time Out
     Verify,Identify,Enroll,Enroll One Time,Enroll And Store in RAM,
     Get Feature Data of Captured FP,Verify Downloaded Feature with Captured FP,
     Identify Downloaded Feature with Captured FP,Identify Free

     In the period of above command executing,the parameter of the Finger TimeOut
     is the time limit of detect finger on sensor repeatedly
  [Parameter]
  	 fingerTimeout - 0-60s can be selected, Default is 5s

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the value of TimeOut is out off range,result=ERR_INVALID_TIME_OUT
    2) else update the value of TimeOut and response the command
*/
bool FPS_CAMA::setFingerTimeout(word fingerTimeout)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setFingerTimeout");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SetFingerTimeOut,2);
	cp->addWordToCommandData(0, fingerTimeout);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SetFingerTimeOut);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            if (rp->wordFromBytes(rp->dataBytes,0) == fingerTimeout) res = true;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setFingerTimeout Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Finger Time Out
     Read Value of Finger Time Out from module

  [Parameter]
  	 None

  [Return value]
  	 0-60s, Default is 5s

  [Operation Sequence]

*/
word FPS_CAMA::getFingerTimeout()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getFingerTimeout");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetFingerTimeOut,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetFingerTimeOut);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = rp->wordFromBytes(rp->dataBytes,0);
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getFingerTimeout Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Set Device ID
     Set Device ID Number

  [Parameter]
  	 deviceId - 1-254 can be selected, Default is 1

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	1) if the value is out off range,result=ERR_INVALID_PARAM
    2) else update the value of Device ID and response the command
*/
bool FPS_CAMA::setDeviceId(word deviceId)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setDeviceId");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SetDeviceId,2);
	cp->addWordToCommandData(0, deviceId);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SetDeviceId);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            if (rp->wordFromBytes(rp->dataBytes,0) == deviceId) res = true;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setDeviceId Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Device ID
     Read Device ID from Module

  [Parameter]
  	 None

  [Return value]
  	 1-254, Default is 1

  [Operation Sequence]

*/
word FPS_CAMA::getDeviceId()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getDeviceId");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetDeviceId,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetDeviceId);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = rp->wordFromBytes(rp->dataBytes,0);
        }
        else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getDeviceId Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get F/W Version
     Get Firmware Version of the Module

  [Parameter]
  	 None

  [Return value]
  	 version as word, high byte is Major, low byte is Minor

  [Operation Sequence]

*/
word FPS_CAMA::getFirmwareVersion()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getFirmwareVersion");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetFWVersion,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetFWVersion);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = rp->wordFromBytes(rp->dataBytes,0);
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getFirmwareVersion Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Finger Detect
     Detect whether finger press on the sensor or not

  [Parameter]
  	 None

  [Return value]
  	 true/false

  [Operation Sequence]
    Notice:
    Host need send Sensor LED Control enable command to power on LED before
    Finger Detect command, otherwise the result of Finger Detect is incorrect

*/
bool FPS_CAMA::fingerDetect()
{
	word res = false;
	if (useSerialDebug) Serial.println("FPS - fingerDetect");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::FingerDetect,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::FingerDetect);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = (rp->wordFromBytes(rp->dataBytes,0) == 1);
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - fingerDetect Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Set Baudrate
     Set UART Baudrate

  [Parameter]
  	 baudrate:
        1 : 9600bps
        2 : 19200bps
        3 : 38400bps
        4 : 57600bps
        5 : 115200bps

  [Return value]
  	 true on success, false on error

  [Operation Sequence]
	Notice: New value is active only by reset or power on
*/
bool FPS_CAMA::setBaudrate(word baudrate)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setBaudrate");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SetBaudRate,2);
	cp->addWordToCommandData(0, baudrate);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SetBaudRate);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            if (rp->wordFromBytes(rp->dataBytes,0) == baudrate) res = true;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setBaudrate Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Set Duplication Check(ON/OFF)
     Setup Enable/Disable fingerprint duplication check in the period of
     “Enroll” or ”Enroll One Time” command

  [Parameter]
  	 duplicationCheck - true/false

  [Return value]
  	 true on success, false on error

  [Operation Sequence]

*/
bool FPS_CAMA::setDuplicationCheck(bool duplicationCheck)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setDuplicationCheck");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SetDuplicationCheck,2);
	cp->addWordToCommandData(0, (duplicationCheck ? 1 : 0));

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SetDuplicationCheck);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            if ((rp->wordFromBytes(rp->dataBytes,0) == 1 ? true : false) == duplicationCheck) res = true;
		}
		else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setDuplicationCheck Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Duplication Check
     Get Status of Duplication Check

  [Parameter]
  	 None

  [Return value]
  	 1-254, Default is 1

  [Operation Sequence]

*/
bool FPS_CAMA::getDuplicationCheck()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getDuplicationCheck");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetDuplicationCheck,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetDuplicationCheck);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = (rp->wordFromBytes(rp->dataBytes,0) == 1);
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getDuplicationCheck Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Enter Standby Mode
     Put module into standby mode
     Only reset or power on will lead module from standby mode to active mode

  [Parameter]
  	 None

  [Return value]
  	 true/false

  [Operation Sequence]
    1) Module enter into Standby Mode and result=ERR_SUCCESS。
    2) From standby mode to active mode only by RESET or Power ON

    Note: Before power off module, Enter Standby Mode Command is recommand
*/
bool FPS_CAMA::standbyMode()
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - standbyMode");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::EnterStandbyMode,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::EnterStandbyMode);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength == 0)) {
            res = true;
        }
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - standbyMode Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Sensor LED Control
     ON/OFF LED of fingerprint sensor

  [Parameter]
  	 duplicationCheck - true/false

  [Return value]
  	 true on success, false on error

  [Operation Sequence]

*/
bool FPS_CAMA::setSensorLed(bool ledEnabled)
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - setSensorLed");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::SensorLEDControl,2);
	cp->addWordToCommandData(0, (ledEnabled ? 1 : 0));

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::SensorLEDControl);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if (! rp->isError) {
            res = true;
		}
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - setSensorLed Result=");
		Serial.print(res);
		Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Get Enroll Count
     Get total number of fingerprint template that have enrolled

  [Parameter]
  	 None

  [Return value]
  	 number of enrolled fingerprint templates

  [Operation Sequence]

*/
word FPS_CAMA::getEnrollCount()
{
	word res = 0;
	if (useSerialDebug) Serial.println("FPS - getEnrollCount");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::GetEnrollCount,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;

	Response_Packet* rp = getResponse(Command_Packet::Commands::GetEnrollCount);

	lastResultCode = 0xFFFF;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength > 0)) {
            res = rp->wordFromBytes(rp->dataBytes,0);
        }
        else lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getEnrollCount Result =");
		Serial.print(res);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     FP Cancel

     1) In the period of process following command:
            Verify
            Identify
            Enroll
            Enroll One Time
            Enroll And Store in RAM
            Get Feature Data of Captured FP
            Verify Downloaded Feature with Captured FP
            Identify Downloaded Feature With Captured FP

            Once received command of FP Cancel,stop the command in process immediately,
            then return 2 response packet :
            one is the result=ERR_FP_CANCEL indicate current command have been cancelled.
            The other result=ERR_SUCCESS denote that operation of cancel is successful.
     2) for the other command except above ○1 ,only return one response packet that is
            Result= ERR_SUCCESS denote that operation of cancel is successful

  [Parameter]
  	 None

  [Return value]
  	 true/false

  [Operation Sequence]
    1) Module enter into Standby Mode and result=ERR_SUCCESS。
    2) From standby mode to active mode only by RESET or Power ON

    Note: Before power off module, Enter Standby Mode Command is recommand
*/
void FPS_CAMA::sendFPCancel()
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - sendFPCancel");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::FPCancel,0);

	byte* packetbytes = cp->getPacketBytes();
	sendCommand(packetbytes, 24);
	delete packetbytes;
	delete cp;
}

bool FPS_CAMA::getFPCancelResult()
{
	bool res = false;
	if (useSerialDebug) Serial.println("FPS - getFPCancelResult");

	Response_Packet* rp = getResponse(Command_Packet::Commands::FPCancel);

	if (rp->validResponse) {
		if (! rp->isError) {
            res = true;
        }
        else lastResultCode = rp->resultCode;
	}
	delete rp;

	if (useSerialDebug) {
		Serial.print("FPS - getFPCancelResult Result =");
		Serial.print(res);
        Serial.print(", current ResultCode=0x");
		Serial.print(rp->resultCode, HEX);
        Serial.print(", lastResultCode=0x");
		Serial.print(lastResultCode, HEX);
		Serial.println();
	}

	return res;
}

/*
  [Function]
     Test Connection
     Test connection between host and module

  [Parameter]
  	 None

  [Return value]
  	 true/false

  [Operation Sequence]
*/
bool FPS_CAMA::testConnection(bool retry)
{
    byte retryCount=3;
    bool res = false;
	if (useSerialDebug) Serial.println("FPS - testConnection");

	Command_Packet* cp = new Command_Packet(Command_Packet::Commands::TestConnection,0);

	byte* packetbytes = cp->getPacketBytes();
    bool done = false;
    for (byte i=0;i<retryCount;i++) {
    	sendCommand(packetbytes, 24);

    	Response_Packet* rp = getResponse(Command_Packet::Commands::TestConnection);

    	lastResultCode = 0xFFFF;
    	if (rp->validResponse) {
    		lastResultCode = rp->resultCode;
    		if (! rp->isError) {
                res = true;
            }
            done=true;
    	}
        else if (retry) {
            done = false;
            if (useSerialDebug) {
                Serial.print("Response invalid - read one more response if data is there and try again (");
                Serial.print(i);
                Serial.println(")");
            }
            // we read what's there and then try again incl. request
            Response_Packet* rp2 = readAvailableResponse();
            delete rp2;
            continue;
        }
        else {
            done=true;
        }
    	delete rp;

    	if (useSerialDebug) {
    		Serial.print("FPS - testConnection Result =");
    		Serial.print(res);
            Serial.print(", lastResultCode=0x");
    		Serial.print(lastResultCode, HEX);
    		Serial.println();
    	}
        if (done) break;
    }
    delete packetbytes;
    delete cp;

	return res;
}

// Sends the command to the software serial channel
//DONE
void FPS_CAMA::sendCommand(byte cmd[], int length)
{
	_serial->write(cmd, length);
	if (useSerialDebug)
	{
		Serial.print("FPS - SEND: ");
		sendToSerial(cmd, length);
		Serial.println();
	}
};

// Gets the response to the command from the software serial channel (and waits for it)
//DONE
Response_Packet* FPS_CAMA::getResponse(Command_Packet::Commands::Commands_Enum awaitedResponseCode)
{
	byte firstbyte = 0;
	bool done = false;
    unsigned long timeoutTime = millis() + serialTimeout;
    byte* resp = new byte[24];
	while (done == false)
	{
        while ((_serial->available() == false) && (millis()<=timeoutTime)) delay(50);
		firstbyte = (byte)_serial->read();
		if (firstbyte == Response_Packet::RESPONSE_START_CODE_1)
		{
			done = true;
		}
        if (millis()>timeoutTime) break;
	}
    if (done) {
        resp[0] = firstbyte;
    	for (int i=1; i < 24; i++)
    	{
    		while ((_serial->available() == false) && (millis()<=timeoutTime)) delay(50);
            if (millis()>timeoutTime) {
                done = false;
                break;
            }
            resp[i]= (byte) _serial->read();
    	}

    	if (useSerialDebug)
    	{
    		Serial.print("FPS - RECV: ");
    		sendToSerial(resp, 24);
    		Serial.println();
    	}
    }
    if (!done) {
        resp = NULL;
    }

	Response_Packet* rp = new Response_Packet(resp, awaitedResponseCode, useSerialDebug);
	delete resp;

	if (useSerialDebug)
	{
		Serial.println("FPS - Response: ");
		Serial.print("validResponse= ");
		Serial.println(rp->validResponse);
		Serial.print("resultCode= ");
		Serial.println(rp->resultCode);
		Serial.print("(real) dataLength= ");
		Serial.println(rp->dataLength);
		Serial.print("dataBytes: ");
		sendToSerial(rp->dataBytes, 14);
		Serial.println();
	}

	return rp;
};

// Gets the response to the command from the software serial channel if one is there
// returns NULL when not or invalid
//DONE
Response_Packet* FPS_CAMA::readAvailableResponse()
{
    if (!_serial->available()) return NULL;

    byte firstbyte = 0;
	bool done = false;
	while ((done == false) || (!_serial->available()))
	{
		firstbyte = (byte)_serial->read();
		if (firstbyte == Response_Packet::RESPONSE_START_CODE_1)
		{
			done = true;
		}
	}
    if (!_serial->available()) return NULL;

	byte* resp = new byte[24];
	resp[0] = firstbyte;
    int i;
	for (i=1; i < 24; i++)
	{
		resp[i]= (byte) _serial->read();
        if (!_serial->available()) break;
	}
    if (i<24) return NULL;

	if (useSerialDebug)
	{
		Serial.print("FPS - RECV: ");
		sendToSerial(resp, 24);
		Serial.println();
	}

	Response_Packet* rp = new Response_Packet(resp, 0xFFFF, useSerialDebug);
	delete resp;

	if (useSerialDebug)
	{
		Serial.println("FPS - Response: ");
		Serial.print("validResponse= ");
		Serial.println(rp->validResponse);
		Serial.print("resultCode= ");
		Serial.println(rp->resultCode);
		Serial.print("(real) dataLength= ");
		Serial.println(rp->dataLength);
		Serial.print("dataBytes: ");
		sendToSerial(rp->dataBytes, 14);
		Serial.println();
	}

	return rp;
};


// sends the bye aray to the serial debugger in our hex format EX: "00 AF FF 10 00 13"
//DONE
void FPS_CAMA::sendToSerial(byte data[], int length)
{
  boolean first=true;
  Serial.print("\"");
  for(int i=0; i<length; i++)
  {
    if (first) first=false; else Serial.print(" ");
    serialPrintHex(data[i]);
  }
  Serial.print("\"");
}

// sends a byte to the serial debugger in the hex format we want EX "0F"
//DONE
void FPS_CAMA::serialPrintHex(byte data)
{
  char tmp[16];
  sprintf(tmp, "%.2X",data);
  Serial.print(tmp);
}

void FPS_CAMA::setUserActionCallback(FPSUserActionCallbackFunction callback)
{
	userActionCallback = callback;
}

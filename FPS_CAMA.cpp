/* 
	FPS_CAMA.h v1.0 - Library for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, December 2015
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
	TLDR; Wil Wheaton's Law
*/

#include "FPS_CAMA.h";

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
	validResponse = true;
	validResponse &= checkParsing(buffer[0], COMMAND_START_CODE_1, COMMAND_START_CODE_1, "COMMAND_START_CODE_1", useSerialDebug);
	validResponse &= checkParsing(buffer[1], COMMAND_START_CODE_2, COMMAND_START_CODE_2, "COMMAND_START_CODE_2", useSerialDebug);

	word checksum = calculateChecksum(buffer, 22);
	byte checksum_low = getLowByte(checksum);
	byte checksum_high = getHighByte(checksum);
	validResponse &= checkParsing(buffer[22], checksum_low, checksum_low, "Checksum_LOW", useSerialDebug);
	validResponse &= checkParsing(buffer[23], checksum_high, checksum_high, "Checksum_HIGH", useSerialDebug);

	byte responseCode_low=getLowByte(awaitedResponseCode);
	byte responseCode_high=getHighByte(awaitedResponseCode);
	validResponse &= checkParsing(buffer[2], responseCode_low, responseCode_low, "ResponseCode_HIGH", useSerialDebug);
	validResponse &= checkParsing(buffer[3], responseCode_high, responseCode_high, "ResponseCode_LOW", useSerialDebug);
	
	dataLength=wordFromBytes(buffer,4)-2;
	
	word errorRet = wordFromBytes(buffer,6);
	if (errorRet==0) isError=false;
	  else if (errorRet==1) isError=true;
	  else validResponse=false;
		
	memset(dataBytes, 0, sizeof(dataBytes)); // Clear DataBytes Array
	if (dataLength>0) {
		for (int i=0; i < dataLength; i++)
		{
			dataBytes[i]=buffer[8+i];
		}
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



#pragma region -= Data_Packet =- 
//void Data_Packet::StartNewPacket()
//{
//	Data_Packet::NextPacketID = 0;
//	Data_Packet::CheckSum = 0;
//}
#pragma endregion

#pragma region -= FPS_CAMA Definitions =-

// Creates a new object to interface with the fingerprint scanner
FPS_CAMA::FPS_CAMA(Stream& streamInstance)
	: _serial(&streamInstance)
{
  // nothing to do
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

	lastResultCode = NULL;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::Verify, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	//TODO FPCancel!
				    } 
				}
				if (! cancel) {
					Response_Packet* rp2 = getResponse(Command_Packet::Commands::Verify);
					lastResultCode = NULL;
					if (rp2->validResponse) {
						if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
						if ((! rp2->isError) && (rp2->dataLength == 2)) {
							if (rp2->wordFromBytes(rp2->dataBytes,0) == templateId) res=true;
							  else lastResultCode=Response_Packet::ResultCodes::ERR_FAIL;
						}
					}
					delete rp2;
				}
			}
		}
	}	
	delete rp1;
	
	if (useSerialDebug) {
		Serial.print("FPS - verify Result=");
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

	lastResultCode = NULL;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::Identify, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	//TODO FPCancel!
				    } 
				}
				if (! cancel) {
					Response_Packet* rp2 = getResponse(Command_Packet::Commands::Identify);
					lastResultCode = NULL;
					if (rp2->validResponse) {
						if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
						if ((! rp2->isError) && (rp2->dataLength == 2)) {
							res=rp2->wordFromBytes(rp2->dataBytes,0);
						}
					}
					delete rp2;
				}
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

	while (true) {
		Response_Packet* rp = getResponse(Command_Packet::Commands::Enroll);

		lastResultCode = NULL;
		if (rp->validResponse) {
			if (rp->dataLength > 0) lastResultCode = rp->wordFromBytes(rp->dataBytes,0);
			bool awaitReleaseFinger=false;
			bool awaitFinalResponse=false;
			if (! rp->isError) {
				if ((awaitFinalResponse) && (lastResultCode != Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) {
					if (rp->dataLength > 0) lastResultCode = rp->resultCode;
					if ((! rp->isError) && (rp->dataLength == 2)) {
						if (rp->wordFromBytes(rp->dataBytes,0) == templateId) res=true;
							else lastResultCode=Response_Packet::ResultCodes::ERR_VERIFY;

					}
					break;
				}
				else if ((lastResultCode == Response_Packet::ResultCodes::GD_NEED_FIRST_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_SECOND_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_THIRD_SWEEP) ||
				         (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) {
					if (((awaitReleaseFinger) && (lastResultCode != Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER)) ||
					    ((! awaitReleaseFinger) && (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER))) {
					  // ERROR in Process
					  // TODO FPCancel senden
					  break;
					}
					
					bool cancel=false;
					if (userActionCallback) {
					    cancel=(! userActionCallback(Command_Packet::Commands::Enroll, Response_Packet::ResultCodes::parseFromBytes(lastResultCode)));
					    if (cancel) {
					    	//TODO FPCancel!
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
					// TODO FPCancel senden
					break;
				}
			}
			else {
				break;
			}
		}
		delete rp;
	}	
	
	if (useSerialDebug) {
		Serial.print("FPS - enroll Result templateId=");
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
     Enroll command require the same finger press on the sensor for 3 times,Corresponding 
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

	lastResultCode = NULL;
	if (rp1->validResponse) {
		if (rp1->dataLength > 0) lastResultCode = rp1->wordFromBytes(rp1->dataBytes,0);
		if (! rp1->isError) {
			if (lastResultCode == Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER) {
				bool cancel=false;
				if (userActionCallback) {
				    cancel=(! userActionCallback(Command_Packet::Commands::EnrollOneTime, Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER));
				    if (cancel) {
				    	//TODO FPCancel!
				    } 
				}
				if (! cancel) {
					Response_Packet* rp2 = getResponse(Command_Packet::Commands::EnrollOneTime);
					lastResultCode = NULL;
					if (rp2->validResponse) {
						if (rp2->dataLength > 0) lastResultCode = rp2->resultCode;
						if ((! rp2->isError) && (rp2->dataLength == 2)) {
							if (rp2->wordFromBytes(rp2->dataBytes,0) == templateId) res=true;
							  else lastResultCode=Response_Packet::ResultCodes::ERR_FAIL;
						}
					}
					delete rp2;
				}
			}
		}
	}	
	delete rp1;
	
	if (useSerialDebug) {
		Serial.print("FPS - enrollOneTime Result=");
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

	lastResultCode = NULL;
	if (rp->validResponse) {
		if (rp->dataLength > 0) lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength == 2)) {
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

	lastResultCode = NULL;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength == 2)) res =  rp->wordFromBytes(rp->dataBytes,0);
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

	lastResultCode = NULL;
	if (rp->validResponse) {
		lastResultCode = rp->resultCode;
		if ((! rp->isError) && (rp->dataLength == 2)) res =  rp->wordFromBytes(rp->dataBytes,0);
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
// According to the DataSheet, this does nothing... 
// Implemented it for completeness.
void FPS_CAMA::Close()
{
	if (UseSerialDebug) Serial.println("FPS - Close");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Close;
	cp->Parameter[0] = 0x00;
	cp->Parameter[1] = 0x00;
	cp->Parameter[2] = 0x00;
	cp->Parameter[3] = 0x00;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	delete rp;
	delete packetbytes;
};

// Turns on or off the LED backlight
// Parameter: true turns on the backlight, false turns it off
// Returns: True if successful, false if not
bool FPS_CAMA::SetLED(bool on)
{
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::CmosLed;
	if (on)
	{
		if (UseSerialDebug) Serial.println("FPS - LED on");
		cp->Parameter[0] = 0x01;
	}
	else
	{
		if (UseSerialDebug) Serial.println("FPS - LED off");
		cp->Parameter[0] = 0x00;
	}
	cp->Parameter[1] = 0x00;
	cp->Parameter[2] = 0x00;
	cp->Parameter[3] = 0x00;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	bool retval = true;
	if (rp->ACK == false) retval = false;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
};

// Changes the baud rate of the connection
// Parameter: 9600, 19200, 38400, 57600, 115200
// Returns: True if success, false if invalid baud
// NOTE: Untested (don't have a logic level changer and a voltage divider is too slow)
bool FPS_CAMA::ChangeBaudRate(int baud)
{
	if ((baud == 9600) || (baud == 19200) || (baud == 38400) || (baud == 57600) || (baud == 115200))
	{

		if (UseSerialDebug) Serial.println("FPS - ChangeBaudRate");
		Command_Packet* cp = new Command_Packet();
		cp->Command = Command_Packet::Commands::Open;
		cp->ParameterFromInt(baud);
		byte* packetbytes = cp->GetPacketBytes();
		SendCommand(packetbytes, 12);
		Response_Packet* rp = GetResponse();
		bool retval = rp->ACK;
		if (retval) 
		{
			_serial.end();
			_serial.begin(baud);
		}
		delete rp;
		delete packetbytes;
		return retval;
	}
	return false;
}

// Gets the number of enrolled fingerprints
// Return: The total number of enrolled fingerprints
int FPS_CAMA::GetEnrollCount()
{
	if (UseSerialDebug) Serial.println("FPS - GetEnrolledCount");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::GetEnrollCount;
	cp->Parameter[0] = 0x00;
	cp->Parameter[1] = 0x00;
	cp->Parameter[2] = 0x00;
	cp->Parameter[3] = 0x00;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();

	int retval = rp->IntFromParameter();
	delete rp;
	delete packetbytes;
	return retval;
}

// checks to see if the ID number is in use or not
// Parameter: 0-199
// Return: True if the ID number is enrolled, false if not
bool FPS_CAMA::CheckEnrolled(int id)
{
	if (UseSerialDebug) Serial.println("FPS - CheckEnrolled");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::CheckEnrolled;
	cp->ParameterFromInt(id);
	byte* packetbytes = cp->GetPacketBytes();
	delete cp;
	SendCommand(packetbytes, 12);
	delete packetbytes;
	Response_Packet* rp = GetResponse();
	bool retval = false;
	retval = rp->ACK;
	delete rp;
	return retval;
}

// Starts the Enrollment Process
// Parameter: 0-199
// Return:
//	0 - ACK
//	1 - Database is full
//	2 - Invalid Position
//	3 - Position(ID) is already used
int FPS_CAMA::EnrollStart(int id)
{
	if (UseSerialDebug) Serial.println("FPS - EnrollStart");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::EnrollStart;
	cp->ParameterFromInt(id);
	byte* packetbytes = cp->GetPacketBytes();
	delete cp;
	SendCommand(packetbytes, 12);
	delete packetbytes;
	Response_Packet* rp = GetResponse();
	int retval = 0;
	if (rp->ACK == false)
	{
		if (rp->Error == Response_Packet::ErrorCodes::NACK_DB_IS_FULL) retval = 1;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_INVALID_POS) retval = 2;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_IS_ALREADY_USED) retval = 3;
	}
	delete rp;
	return retval;
}

// Gets the first scan of an enrollment
// Return: 
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//	3 - ID in use
int FPS_CAMA::Enroll1()
{
	if (UseSerialDebug) Serial.println("FPS - Enroll1");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Enroll1;
	byte* packetbytes = cp->GetPacketBytes();
	delete cp;
	SendCommand(packetbytes, 12);
	delete packetbytes;
	Response_Packet* rp = GetResponse();
	int retval = rp->IntFromParameter();
	if (retval < 200) retval = 3; else retval = 0;
	if (rp->ACK == false)
	{
		if (rp->Error == Response_Packet::ErrorCodes::NACK_ENROLL_FAILED) retval = 1;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_BAD_FINGER) retval = 2;
	}
	delete rp;
	if (rp->ACK) return 0; else return retval;
}

// Gets the Second scan of an enrollment
// Return: 
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//	3 - ID in use
int FPS_CAMA::Enroll2()
{
	if (UseSerialDebug) Serial.println("FPS - Enroll2");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Enroll2;
	byte* packetbytes = cp->GetPacketBytes();
	delete cp;
	SendCommand(packetbytes, 12);
	delete packetbytes;
	Response_Packet* rp = GetResponse();
	int retval = rp->IntFromParameter();
	if (retval < 200) retval = 3; else retval = 0;
	if (rp->ACK == false)
	{
		if (rp->Error == Response_Packet::ErrorCodes::NACK_ENROLL_FAILED) retval = 1;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_BAD_FINGER) retval = 2;
	}
	delete rp;
	if (rp->ACK) return 0; else return retval;
}

// Gets the Third scan of an enrollment
// Finishes Enrollment
// Return: 
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//	3 - ID in use
int FPS_CAMA::Enroll3()
{
	if (UseSerialDebug) Serial.println("FPS - Enroll3");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Enroll3;
	byte* packetbytes = cp->GetPacketBytes();
	delete cp;
	SendCommand(packetbytes, 12);
	delete packetbytes;
	Response_Packet* rp = GetResponse();
	int retval = rp->IntFromParameter();
	if (retval < 200) retval = 3; else retval = 0;
	if (rp->ACK == false)
	{
		if (rp->Error == Response_Packet::ErrorCodes::NACK_ENROLL_FAILED) retval = 1;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_BAD_FINGER) retval = 2;
	}
	delete rp;
	if (rp->ACK) return 0; else return retval;
}

// Checks to see if a finger is pressed on the FPS
// Return: true if finger pressed, false if not
bool FPS_CAMA::IsPressFinger()
{
	if (UseSerialDebug) Serial.println("FPS - IsPressFinger");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::IsPressFinger;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	bool retval = false;
	int pval = rp->ParameterBytes[0];
	pval += rp->ParameterBytes[1];
	pval += rp->ParameterBytes[2];
	pval += rp->ParameterBytes[3];
	if (pval == 0) retval = true;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
}

// Deletes the specified ID (enrollment) from the database
// Parameter: 0-199 (id number to be deleted)
// Returns: true if successful, false if position invalid
bool FPS_CAMA::DeleteID(int id)
{
	if (UseSerialDebug) Serial.println("FPS - DeleteID");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::DeleteID;
	cp->ParameterFromInt(id);
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	bool retval = rp->ACK;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
}

// Deletes all IDs (enrollments) from the database
// Returns: true if successful, false if db is empty
bool FPS_CAMA::DeleteAll()
{
	if (UseSerialDebug) Serial.println("FPS - DeleteAll");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::DeleteAll;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	bool retval = rp->ACK;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
}

// Checks the currently pressed finger against a specific ID
// Parameter: 0-199 (id number to be checked)
// Returns:
//	0 - Verified OK (the correct finger)
//	1 - Invalid Position
//	2 - ID is not in use
//	3 - Verified FALSE (not the correct finger)
int FPS_CAMA::Verify1_1(int id)
{
	if (UseSerialDebug) Serial.println("FPS - Verify1_1");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Verify1_1;
	cp->ParameterFromInt(id);
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	int retval = 0;
	if (rp->ACK == false)
	{
		retval = 3; // grw 01/03/15 - set default value of not verified before assignment
		if (rp->Error == Response_Packet::ErrorCodes::NACK_INVALID_POS) retval = 1;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_IS_NOT_USED) retval = 2;
		if (rp->Error == Response_Packet::ErrorCodes::NACK_VERIFY_FAILED) retval = 3;
	}
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
}

// Checks the currently pressed finger against all enrolled fingerprints
// Returns:
//	0-199: Verified against the specified ID (found, and here is the ID number)
//	200: Failed to find the fingerprint in the database
int FPS_CAMA::Identify1_N()
{
	if (UseSerialDebug) Serial.println("FPS - Identify1_N");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::Identify1_N;
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	int retval = rp->IntFromParameter();
	if (retval > 200) retval = 200;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;
}

// Captures the currently pressed finger into onboard ram use this prior to other commands
// Parameter: true for high quality image(slower), false for low quality image (faster)
// Generally, use high quality for enrollment, and low quality for verification/identification
// Returns: True if ok, false if no finger pressed
bool FPS_CAMA::CaptureFinger(bool highquality)
{
	if (UseSerialDebug) Serial.println("FPS - CaptureFinger");
	Command_Packet* cp = new Command_Packet();
	cp->Command = Command_Packet::Commands::CaptureFinger;
	if (highquality)
	{
		cp->ParameterFromInt(1);
	}
	else
	{
		cp->ParameterFromInt(0);
	}
	byte* packetbytes = cp->GetPacketBytes();
	SendCommand(packetbytes, 12);
	Response_Packet* rp = GetResponse();
	bool retval = rp->ACK;
	delete rp;
	delete packetbytes;
	delete cp;
	return retval;

}
#pragma endregion

#pragma region -= Not imlemented commands =-
// Gets an image that is 258x202 (52116 bytes) and returns it in 407 Data_Packets
// Use StartDataDownload, and then GetNextDataPacket until done
// Returns: True (device confirming download starting)
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//bool FPS_CAMA::GetImage()
//{
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//return false;
//}

// Gets an image that is qvga 160x120 (19200 bytes) and returns it in 150 Data_Packets
// Use StartDataDownload, and then GetNextDataPacket until done
// Returns: True (device confirming download starting)
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//bool FPS_CAMA::GetRawImage()
//{
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//return false;
//}

// Gets a template from the fps (498 bytes) in 4 Data_Packets
// Use StartDataDownload, and then GetNextDataPacket until done
// Parameter: 0-199 ID number
// Returns: 
//	0 - ACK Download starting
//	1 - Invalid position
//	2 - ID not used (no template to download
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//int FPS_CAMA::GetTemplate(int id)
//{
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//return false;
//}

// Uploads a template to the fps 
// Parameter: the template (498 bytes)
// Parameter: the ID number to upload
// Parameter: Check for duplicate fingerprints already on fps
// Returns: 
//	0-199 - ID duplicated
//	200 - Uploaded ok (no duplicate if enabled)
//	201 - Invalid position
//	202 - Communications error
//	203 - Device error
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//int FPS_CAMA::SetTemplate(byte* tmplt, int id, bool duplicateCheck)
//{
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//return -1;
//}

// resets the Data_Packet class, and gets ready to download
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//void FPS_CAMA::StartDataDownload()
//{
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//}

// Returns the next data packet 
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
//Data_Packet GetNextDataPacket()
//{
//	return 0;
//}

// Commands that are not implemented (and why)
// VerifyTemplate1_1 - Couldn't find a good reason to implement this on an arduino
// IdentifyTemplate1_N - Couldn't find a good reason to implement this on an arduino
// MakeTemplate - Couldn't find a good reason to implement this on an arduino
// UsbInternalCheck - not implemented - Not valid config for arduino
// GetDatabaseStart - historical command, no longer supported
// GetDatabaseEnd - historical command, no longer supported
// UpgradeFirmware - Data sheet says not supported
// UpgradeISOCDImage - Data sheet says not supported
// SetIAPMode - for upgrading firmware (which data sheet says is not supported)
// Ack and Nack	are listed as commands for some unknown reason... not implemented
#pragma endregion
*/

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
	while (done == false)
	{
		firstbyte = (byte)_serial->read();
		if (firstbyte == Response_Packet::COMMAND_START_CODE_1)
		{
			done = true;
		}
	}
	byte* resp = new byte[24];
	resp[0] = firstbyte;
	for (int i=1; i < 24; i++)
	{
		while (_serial->available() == false) delay(10);
		resp[i]= (byte) _serial->read();
	}
	
	if (useSerialDebug) 
	{
		Serial.print("FPS - RECV: ");
		sendToSerial(resp, 24);
		Serial.println();
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


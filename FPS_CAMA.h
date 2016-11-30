/*
	FPS_CAMA.h v1.0 - Library for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, December 2015
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
	TLDR; Wil Wheaton's Law
*/

#ifndef FPS_CAMA_h
#define FPS_CAMA_h

#include "Arduino.h"
#include "Stream.h"
/*
	Command Packet is the instruction from Host to Target (CAMA-SM Series),
	Total length of the command packet is 24 Bytes
*/
class Command_Packet
{
	public:
		class Commands
		{
			public:
				enum Commands_Enum
				{
					IncorrectCommand						= 0x0000,
					Verify									= 0x0101,
					Identify								= 0x0102,
					Enroll									= 0x0103,
					EnrollOneTime							= 0x0104,
					ClearTemplate							= 0x0105,
					ClearAllTemplate						= 0x0106,
					GetEmptyId								= 0x0107,
					GetTemplateStatus						= 0x0108,
					GetBrokenTemplate						= 0x0109,
					ReadTemplate							= 0x010A,
					WriteTemplate							= 0x010B,
					SetSecurityLevel						= 0x010C,
					GetSecurityLevel						= 0x010D,
					SetFingerTimeOut						= 0x010E,
					GetFingerTimeOut						= 0x010F,
					SetDeviceId								= 0x0110,
					GetDeviceId								= 0x0111,
					GetFWVersion							= 0x0112,
					FingerDetect							= 0x0113,
					SetBaudRate								= 0x0114,
					SetDuplicationCheck						= 0x0115,
					GetDuplicationCheck						= 0x0116,
					EnterStandbyMode						= 0x0117,
					EnrollAndStoreInRam						= 0x0118,
					GetEnrollData							= 0x0119,
					GetFeatureDataofCapturedFP				= 0x011A,
					VerifyDownloadedFeatureWithCapturedFP	= 0x011B,
					IdentifyDownloadedFeatureWithCapturedFP	= 0x011C,
					GetDeviceName							= 0x0121,
					SensorLEDControl						= 0x0124,
					IdentifyFree							= 0x0125,
					SetDevicePassword						= 0x0126,
					VerifyDevicePassword					= 0x0127,
					GetEnrollCount							= 0x0128,
					FPCancel								= 0x0130,
					TestConnection							= 0x0150,
					IncorrectCommandResponse				= 0x0160
			};
		};

		Commands::Commands_Enum command;
		word dataLength;
		byte commandData[16];								// Parameter up to 16 bytes, changes meaning depending on command
		byte* getPacketBytes();								// returns the bytes to be transmitted
		void addByteToCommandData(byte index, byte value);  // add a byte to CommandData array starting at Index index
		void addWordToCommandData(byte index, word value);  // add a word to CommandData array starting at Index index
		void addIntToCommandData(byte index, int value);    // add a int to CommandData array starting at Index index

		Command_Packet(Commands::Commands_Enum _command, word _dataLength);

	private:
		static const byte COMMAND_START_CODE_1 = 0x55;	// Static byte to mark the beginning of a command packet	-	never changes
		static const byte COMMAND_START_CODE_2 = 0xAA;	// Static byte to mark the beginning of a command packet	-	never changes

		word calculateChecksum(byte* data);						// Checksum is calculated using byte addition
		byte getHighByte(word w);
		byte getLowByte(word w);
};

/*
	Response packet is result of execute command packet, from Target (CAMA-SM Series) to Host,
	Total length of the command packet is 24 Bytes
*/
class Response_Packet
{
	public:
		class ResultCodes
		{
			public:
				enum ResultCodes_Enum
				{
					ERR_SUCCESS				= 0x00, // Success for command execute
					ERR_FAIL				= 0x01, // Fail for instruction execute
					ERR_VERIFY				= 0x11, // One to one match fail
					ERR_IDENTIFY			= 0x12, // Identify fail, no matched template
					ERR_TMPL_EMPTY			= 0x13, // The appointed Template Data is Null
					ERR_TMPL_NOT_EMPTY		= 0x14, // Existed template for the appointed ID.
					ERR_ALL_TMPL_EMPTY		= 0x15, // All template is Null, Without enrolled Template.
					ERR_EMPTY_ID_NOEXIST	= 0x16, // Without available Template ID.
					ERR_BROKEN_ID_NOEXIST	= 0x17, // Without damaged Template.
					ERR_INVALID_TMPL_DATA	= 0x18, // Appointed Template Data invalid.
					ERR_DUPLICATION_ID		= 0x19, // The fingerprint has been enrolled.
					ERR_BAD_QUALITY			= 0x21, // Bad quality fingerprint image.
					ERR_TIME_OUT			= 0x23, // During Time Out period, no finger is detected
					ERR_NOT_AUTHORIZED		= 0x24, // Not authorized by the password
													// If set password, and not use Verify Device Password command,
													// then all commands would return error code expect
													// Test Connection, Verify Device Password;
													// If no password, all commands are available without the password.
					ERR_GENERALIZE			= 0x30, // Generalize template data fail
					ERR_FP_CANCEL			= 0x41, // Relative fingerprint command have been cancelled
					ERR_INTERNAL			= 0x50, // Internal error
					ERR_MEMORY				= 0x51, // Memory Error
					ERR_EXCEPTION			= 0x52, // Firmware error.
					ERR_INVALID_TMPL_NO		= 0x60, // Template No. is invalid
					ERR_INVALID_SEC_VAL		= 0x61, // The Value of Security Level is invalid
					ERR_INVALID_TIME_OUT	= 0x62, // The Value of Time Out is invalid
					ERR_INVALID_BAUDRATE	= 0x63, // The Value of Baudrate is invalid
					ERR_DEVICE_ID_EMPTY		= 0x64, // Not setting Device ID
					ERR_INVALID_DUP_VAL		= 0x65, // Option Value of Duplication Check is invalid
					ERR_INVALID_PARAM		= 0x70, // Parameter is invalid.
					ERR_NO_RELEASE			= 0x71, // In process of IdentifyFree command, Finger is not release

					GD_DOWNLOAD_SUCCESS		= 0xA1, // Download Template data was successful

					GD_NEED_FIRST_SWEEP		= 0xFFF1, // Waiting input fingerprint for first time.
					GD_NEED_SECOND_SWEEP	= 0xFFF2, // Waiting input fingerprint for second time.
					GD_NEED_THIRD_SWEEP		= 0xFFF3, // Waiting input fingerprint for third time.
					GD_NEED_RELEASE_FINGER	= 0xFFF4, // Lift finger

 					GD_DETECT_FINGER		= 0x01, // Detect an finger on sensor when execute “Finger Detect” command
					GD_NO_DETECT_FINGER		= 0x00, // Detect no any finger on sensor when execute “Finger Detect” command

					GD_TEMPLATE_NOT_EMPTY	= 0x01, // The appointed Template are not empty
					GD_TEMPLATE_EMPTY		= 0x00, // The appointed Template have been emptied

					INVALID					= 0xFFFF
				};

				static ResultCodes_Enum parseFromBytes(word resultCode);
		};
		Response_Packet(byte* buffer, word awaitedResponseCode, bool useSerialDebug);
		ResultCodes::ResultCodes_Enum resultCode;
		bool isError;
		static const byte COMMAND_START_CODE_1 = 0xAA;	// Static byte to mark the beginning of a command packet	-	never changes
		static const byte COMMAND_START_CODE_2 = 0x55;	// Static byte to mark the beginning of a command packet	-	never changes
		word dataLength;
		byte dataBytes[14];
		bool validResponse;
		int intFromBytes(byte* buffer, byte index);
		word wordFromBytes(byte* buffer, byte index);

	private:
		bool checkParsing(byte b, byte propervalue, byte alternatevalue, char const * varname, bool useSerialDebug);
		word calculateChecksum(byte* buffer, int length);
		byte getHighByte(word w);
		byte getLowByte(word w);
};

#pragma region -= Command_Data_Packet =-
/*
	When length of Command Parameter or Data is larger than 16 Bytes,
	Utilize Data Packet to transmit block Data, the maximum length of Data Packet is 512Bytes
*/
// Data Mule packet for receiving large data(in 128 byte pieces) from the FPS
// This class can only transmit one packet at a time
//class Data_Packet
//{
//public:
//	static int CheckSum;
//	int PacketID;
//	int ValidByteLength;
//	byte Data[128];
//	void StartNewPacket();
//	bool IsLastPacket;
//private:
//	static int NextPacketID;
//};
class Command_Data_Packet
{
	public:
		Command_Data_Packet(byte* buffer, bool UseSerialDebug);
		byte RawBytes[12];
		byte ParameterBytes[4];
		byte ResponseBytes[2];
		bool ACK;
		static const byte COMMAND_START_CODE_1 = 0x5A;	// Static byte to mark the beginning of a command packet	-	never changes
		static const byte COMMAND_START_CODE_2 = 0x5A;	// Static byte to mark the beginning of a command packet	-	never changes
		int IntFromParameter();

	private:
		bool CheckParsing(byte b, byte propervalue, byte alternatevalue, char const * varname, bool UseSerialDebug);
		word CalculateChecksum(byte* buffer, int length);
		byte GetHighByte(word w);
		byte GetLowByte(word w);
};
#pragma endregion

#pragma region -= Response_Data_Packet =-
/*
	When length of Command Parameter or Data is larger than 16 Bytes,
	Utilize Data Packet to transmit block Data, the maximum length of Data Packet is 512Bytes
*/
// Data Mule packet for receiving large data(in 128 byte pieces) from the FPS
// This class can only transmit one packet at a time
//class Data_Packet
//{
//public:
//	static int CheckSum;
//	int PacketID;
//	int ValidByteLength;
//	byte Data[128];
//	void StartNewPacket();
//	bool IsLastPacket;
//private:
//	static int NextPacketID;
//};
class Response_Data_Packet
{
	public:
		Response_Data_Packet(byte* buffer, bool UseSerialDebug);
		byte RawBytes[24];
		static const byte COMMAND_START_CODE_1 = 0xA5;	// Static byte to mark the beginning of a command packet	-	never changes
		static const byte COMMAND_START_CODE_2 = 0x5A;	// Static byte to mark the beginning of a command packet	-	never changes
		int IntFromParameter();

	private:
		bool CheckParsing(byte b, byte propervalue, byte alternatevalue, char const * varname, bool UseSerialDebug);
		word CalculateChecksum(byte* buffer, int length);
		byte GetHighByte(word w);
		byte GetLowByte(word w);
};
#pragma endregion


typedef bool(*FPSUserActionCallbackFunction)(const Command_Packet::Commands::Commands_Enum command, const Response_Packet::ResultCodes::ResultCodes_Enum action);

/*
	Object for controlling the GT-511C3 Finger Print Scanner (FPS)
*/
class FPS_CAMA
{

 public:
	// Enables verbose debug output using hardware Serial
	bool useSerialDebug = false;

	// Last ResultCode
	word lastResultCode = 0xFFFF;

	// Creates a new object to interface with the fingerprint scanner
	FPS_CAMA(Stream& streamInstance);

	// [Function] One to one match
	bool verify(word templateId);

	// [Function] One to many match
	word identify();

	// [Function] Enroll
	bool enroll(word templateId);

	// [Function] Enroll one Time
	bool enrollOneTime(word templateId);

	// [Function] Clear Template
	bool clearTemplate(word templateId);

	// [Function] Clear All Templates
	word clearAllTemplate();

	// [Function] Get Empty Id
	word getEmptyId();




	void serialPrintHex(byte data);
	void sendToSerial(byte data[], int length);

	// resets the Data_Packet class, and gets ready to download
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//void StartDataDownload();

	// Returns the next data packet
	// Not implemented due to memory restrictions on the arduino
	// may revisit this if I find a need for it
	//Data_Packet GetNextDataPacket();

	 void setUserActionCallback(FPSUserActionCallbackFunction callback);

private:
	 void sendCommand(byte cmd[], int length);
	 Response_Packet* getResponse(Command_Packet::Commands::Commands_Enum awaitedResponseCode);
	 Stream* _serial;
	 FPSUserActionCallbackFunction userActionCallback = 0;
};

#endif

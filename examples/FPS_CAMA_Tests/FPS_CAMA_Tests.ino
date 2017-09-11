/*
	FPS_CAMA_Tests.ino - Library example for controlling the CAMA Finger Print Scanner (FPS)
	Created by Ingo Fischer, August 02rd 2017

	This sketch acts as unit tests and execute all library functions against simulated results
*/
#include <FPS_CAMA.h>
#include <MemStream.h>
#include <MemoryFree.h>
#include <avr/pgmspace.h>


uint8_t streamBuffer[600];
MemStream testStream(streamBuffer,600,0,false);
FPS_CAMA cama(testStream);

bool fpUserActionReturnValue=true;
bool FPSUserAction(const Command_Packet::Commands::Commands_Enum command, const Response_Packet::ResultCodes::ResultCodes_Enum action) {
  Serial.println();
  Serial.print(F("Command=0x"));
  Serial.print(command,HEX);
  Serial.print(F(", action=0x"));
  Serial.print(action,HEX);
  switch(action) {
    case Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER: Serial.print(F(" - NEED RELEASE FINGER")); break; // Verify, Identify, Enroll
    case Response_Packet::ResultCodes::GD_NEED_FIRST_SWEEP: Serial.print(F(" - NEED FIRST SWEEP")); break; // Enroll
    case Response_Packet::ResultCodes::GD_NEED_SECOND_SWEEP: Serial.print(F(" - NEED SECOND SWEEP")); break; // Enroll
    case Response_Packet::ResultCodes::GD_NEED_THIRD_SWEEP: Serial.print(F(" - NEED THIRD SWEEP")); break; // Enroll
    default: Serial.println(F(" -- UNKNOWN ACTION!!!! -- "));
  }
  Serial.println();
  return fpUserActionReturnValue;
}

bool testVerify() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'VerifySuccess' ..."));
  const static PROGMEM uint8_t buf_VerifySuccess[48] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_VerifySuccess,48);
  testsOk &= (cama.verify(1) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'VerifyError' ..."));
  const static PROGMEM uint8_t buf_VerifyError[24] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_VerifyError,24);
  testsOk &= (cama.verify(1) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'VerifyErrorInvTmplId' ..."));
  const static PROGMEM uint8_t buf_VerifyErrorInvTmplId[48] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_VerifyErrorInvTmplId,48);
  testsOk &= (cama.verify(1) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testIdentify() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'IdentifySuccess' ..."));
  const static PROGMEM uint8_t buf_IdentifySuccess[48] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_IdentifySuccess,48);
  testsOk &= (cama.identify() == 2);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'IdentifyError' ..."));
  const static PROGMEM uint8_t buf_IdentifyError[24] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_IdentifyError,24);
  testsOk &= (cama.identify() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_ALL_TMPL_EMPTY);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testEnroll() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollSuccess' ..."));
  const static PROGMEM uint8_t buf_EnrollSuccess[336] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF2, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollSuccess,336);
  testsOk &= (cama.enroll(3) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollError' ..."));
  const static PROGMEM uint8_t buf_EnrollError[336] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF2, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollError,336);
  testsOk &= (cama.enroll(3) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_GENERALIZE);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollErrorInvTmplId' ..."));
  const static PROGMEM uint8_t buf_EnrollErrorInvTmplId[24] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollErrorInvTmplId,24);
  testsOk &= (cama.enroll(3) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testEnrollOneTime() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollOneTimeSuccess' ..."));
  const static PROGMEM uint8_t buf_EnrollOneTimeSuccess[48] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0x02,
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollOneTimeSuccess,48);
  testsOk &= (cama.enrollOneTime(4) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollOneTimeError' ..."));
  const static PROGMEM uint8_t buf_EnrollOneTimeError[24] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollOneTimeError,24);
  testsOk &= (cama.enrollOneTime(4) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'EnrollOneTimeErrorInvTmplId' ..."));
  const static PROGMEM uint8_t buf_EnrollOneTimeErrorInvTmplId[48] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0x02,
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_EnrollOneTimeErrorInvTmplId,48);
  testsOk &= (cama.enrollOneTime(4) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testClearTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'ClearTemplateSuccess' ..."));
  const static PROGMEM uint8_t buf_ClearTemplateSuccess[24] = {
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_ClearTemplateSuccess,24);
  testsOk &= (cama.clearTemplate(1) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'ClearTemplateError' ..."));
  const static PROGMEM uint8_t buf_ClearTemplateError[24] = {
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_ClearTemplateError,24);
  testsOk &= (cama.clearTemplate(1) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testClearAllTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'ClearAllTemplateSuccess' ..."));
  const static PROGMEM uint8_t buf_ClearAllTemplateSuccess[24] = {
    0xAA, 0x55, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_ClearAllTemplateSuccess,24);
  testsOk &= (cama.clearAllTemplate() == 5);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'ClearAllTemplateError' ..."));
  const static PROGMEM uint8_t buf_ClearAllTemplateError[24] = {
    0xAA, 0x55, 0x06, 0x01, 0x04, 0x00, 0x01, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_ClearAllTemplateError,24);
  testsOk &= (cama.clearAllTemplate() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_EXCEPTION);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetEmptyId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetEmptyIdSuccess' ..."));
  const static PROGMEM uint8_t buf_GetEmptyIdSuccess[24] = {
    0xAA, 0x55, 0x07, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetEmptyIdSuccess,24);
  testsOk &= (cama.getEmptyId() == 2);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetEmptyIdError' ..."));
  const static PROGMEM uint8_t buf_GetEmptyIdError[24] = {
    0xAA, 0x55, 0x07, 0x01, 0x04, 0x00, 0x01, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetEmptyIdError,24);
  testsOk &= (cama.getEmptyId() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_EMPTY_ID_NOEXIST);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetTemplateStatus() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetTemplateStatusUsedSuccess' ..."));
  const static PROGMEM uint8_t buf_GetTemplateStatusUsedSuccess[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetTemplateStatusUsedSuccess,24);
  testsOk &= (cama.getTemplateStatus(1) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::GD_TEMPLATE_NOT_EMPTY);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetTemplateStatusFreeSuccess' ..."));
  const static PROGMEM uint8_t buf_GetTemplateStatusFreeSuccess[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetTemplateStatusFreeSuccess,24);
  testsOk &= (cama.getTemplateStatus(1) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::GD_TEMPLATE_EMPTY);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetTemplateStatusError' ..."));
  const static PROGMEM uint8_t buf_GetTemplateStatusError[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetTemplateStatusError,24);
  testsOk &= (cama.getTemplateStatus(1) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetBrokenTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetBrokenTemplateSuccess' ..."));
  const static PROGMEM uint8_t buf_GetBrokenTemplateSuccess[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetBrokenTemplateSuccess,24);
  testsOk &= (cama.getBrokenTemplate() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetBrokenTemplateSuccess2' ..."));
  const static PROGMEM uint8_t buf_GetBrokenTemplateSuccess2[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetBrokenTemplateSuccess2,24);
  testsOk &= (cama.getBrokenTemplate() == 3);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetBrokenTemplateError' ..."));
  const static PROGMEM uint8_t buf_GetBrokenTemplateError[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x01, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetBrokenTemplateError,24);
  testsOk &= (cama.getBrokenTemplate() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_NOT_AUTHORIZED);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testReadTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'testReadTemplateSuccess' ..."));
  const static PROGMEM uint8_t testStart[34] = {
    0xAA, 0x55, 0x0A, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02,
    0xA5, 0x5A, 0x0A, 0x01, 0xF6, 0x01, 0x00, 0x00, 0x01, 0x00
  };
  uint8_t testChecksum[2] = {0x5E, 0xF3};
  uint8_t* buf_testReadTemplateSuccess = new uint8_t[534];
  int i;
  memset(streamBuffer, 0, 600);
  memcpy_P(streamBuffer, testStart, 34);
  for (i=0; i<498; i++) streamBuffer[34+i]=i%255;
  for (i=0; i<2; i++) streamBuffer[34+498+i]=testChecksum[i];
  testStream.setBufferContentPosition(0, 534);

  uint8_t* testResponse = new uint8_t[498];
  testsOk &= (cama.readTemplate(1, testResponse) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  if (testsOk) {
    for (i=0; i<498; i++) {
      if (testResponse[i] != i%255) {
        testsOk = false;
        Serial.print(F("'testReadTemplateSuccess': Response invalid on index "));
        Serial.print((i+1));
        Serial.print(F(": "));
        Serial.print(testResponse[i]);
        Serial.print(F(" vs. "));
        Serial.println(i%255);
      }
    }
  }
  delete[] testResponse;
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'testReadTemplateFailure' ..."));
  const static PROGMEM uint8_t buf_testReadTemplateFailure[24] = {
    0xAA, 0x55, 0x0A, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_testReadTemplateFailure,24);
  testsOk &= (cama.readTemplate(1, testResponse) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testWriteTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'testWriteTemplateSuccess' ..."));
  const static PROGMEM uint8_t buf_testWriteTemplateSuccess[36] = {
    0xAA, 0x55, 0x0B, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01,
    0xA5, 0x5A, 0x0B, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_testWriteTemplateSuccess,36);

  uint8_t* testTemplate = new uint8_t[498];
  int i;
  memset(testTemplate, 20, 498);
  for (i=0; i<498; i++) testTemplate[i]=i%255;

  testsOk &= (cama.writeTemplate(1, testTemplate) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  delete[] testTemplate;
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  /*Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'testReadTemplateFailure' ..."));
  const static PROGMEM uint8_t buf_testReadTemplateFailure[24] = {
    0xAA, 0x55, 0x0A, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_testReadTemplateFailure,24);
  testsOk &= (cama.readTemplate(1, testResponse) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);*/

  return testsOk;
}
bool testSetSecurityLevel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetSecurityLevelSuccess' ..."));
  const static PROGMEM uint8_t buf_SetSecurityLevelSuccess[24] = {
    0xAA, 0x55, 0x0C, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetSecurityLevelSuccess,24);
  testsOk &= (cama.setSecurityLevel(3) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetSecurityLevelError' ..."));
  const static PROGMEM uint8_t buf_SetSecurityLevelError[24] = {
    0xAA, 0x55, 0x0C, 0x01, 0x04, 0x00, 0x01, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetSecurityLevelError,24);
  testsOk &= (cama.setSecurityLevel(6) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_SEC_VAL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetSecurityLevel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetSecurityLevelSuccess' ..."));
  const static PROGMEM uint8_t buf_GetSecurityLevelSuccess[24] = {
    0xAA, 0x55, 0x0D, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetSecurityLevelSuccess,24);
  testsOk &= (cama.getSecurityLevel() == 3);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetSecurityLevelError' ..."));
  const static PROGMEM uint8_t buf_GetSecurityLevelError[24] = {
    0xAA, 0x55, 0x0D, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetSecurityLevelError,24);
  testsOk &= (cama.getSecurityLevel() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testSetFingerTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetFingerTimeoutSuccess' ..."));
  const static PROGMEM uint8_t buf_SetFingerTimeoutSuccess[24] = {
    0xAA, 0x55, 0x0E, 0x01, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetFingerTimeoutSuccess,24);
  testsOk &= (cama.setFingerTimeout(10) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetFingerTimeoutError' ..."));
  const static PROGMEM uint8_t buf_SetFingerTimeoutError[24] = {
    0xAA, 0x55, 0x0E, 0x01, 0x04, 0x00, 0x01, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetFingerTimeoutError,24);
  testsOk &= (cama.setFingerTimeout(61) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_TIME_OUT);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetFingerTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetFingerTimeoutSuccess' ..."));
  const static PROGMEM uint8_t buf_GetFingerTimeoutSuccess[24] = {
    0xAA, 0x55, 0x0F, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetFingerTimeoutSuccess,24);
  testsOk &= (cama.getFingerTimeout() == 5);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetFingerTimeoutError' ..."));
  const static PROGMEM uint8_t buf_GetFingerTimeoutError[24] = {
    0xAA, 0x55, 0x0F, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetFingerTimeoutError,24);
  testsOk &= (cama.getFingerTimeout() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testSetDeviceId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetDeviceIdSuccess' ..."));
  const static PROGMEM uint8_t buf_SetDeviceIdSuccess[24] = {
    0xAA, 0x55, 0x10, 0x01, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetDeviceIdSuccess,24);
  testsOk &= (cama.setDeviceId(10) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetDeviceIdError' ..."));
  const static PROGMEM uint8_t buf_SetDeviceIdError[24] = {
    0xAA, 0x55, 0x10, 0x01, 0x04, 0x00, 0x01, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetDeviceIdError,24);
  testsOk &= (cama.setDeviceId(266) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_PARAM);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetDeviceId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetDeviceIdSuccess' ..."));
  const static PROGMEM uint8_t buf_GetDeviceIdSuccess[24] = {
    0xAA, 0x55, 0x11, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetDeviceIdSuccess,24);
  testsOk &= (cama.getDeviceId() == 5);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetDeviceIdError' ..."));
  const static PROGMEM uint8_t buf_GetDeviceIdError[24] = {
    0xAA, 0x55, 0x11, 0x01, 0x04, 0x00, 0x01, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetDeviceIdError,24);
  testsOk &= (cama.getDeviceId() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_EXCEPTION);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetFirmwareVersion() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetFirmwareVersionSuccess' ..."));
  const static PROGMEM uint8_t buf_GetFirmwareVersionSuccess[24] = {
    0xAA, 0x55, 0x12, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetFirmwareVersionSuccess,24);
  testsOk &= (cama.getFirmwareVersion() == 0x0902);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testFingerDetect() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'FingerDetectSuccessTrue' ..."));
  const static PROGMEM uint8_t buf_FingerDetectSuccessTrue[24] = {
    0xAA, 0x55, 0x13, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_FingerDetectSuccessTrue,24);
  testsOk &= (cama.fingerDetect() == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'FingerDetectSuccessFalse' ..."));
  const static PROGMEM uint8_t buf_FingerDetectSuccessFalse[24] = {
    0xAA, 0x55, 0x13, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_FingerDetectSuccessFalse,24);
  testsOk &= (cama.fingerDetect() == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testSetBaudrate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetBaudrateSuccess' ..."));
  const static PROGMEM uint8_t buf_SetBaudrateSuccess[24] = {
    0xAA, 0x55, 0x14, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetBaudrateSuccess,24);
  testsOk &= (cama.setBaudrate(1) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetBaudrateError' ..."));
  const static PROGMEM uint8_t buf_SetBaudrateError[24] = {
    0xAA, 0x55, 0x14, 0x01, 0x04, 0x00, 0x01, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetBaudrateError,24);
  testsOk &= (cama.setBaudrate(7) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_BAUDRATE);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testSetDuplicationCheck() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetDuplicationCheckSuccess' ..."));
  const static PROGMEM uint8_t buf_SetDuplicationCheckSuccess[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetDuplicationCheckSuccess,24);
  testsOk &= (cama.setDuplicationCheck(true) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetDuplicationCheckSuccess2' ..."));
  const static PROGMEM uint8_t buf_SetDuplicationCheckSuccess2[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetDuplicationCheckSuccess2,24);
  testsOk &= (cama.setDuplicationCheck(false) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetDuplicationCheckError' ..."));
  const static PROGMEM uint8_t buf_SetDuplicationCheckError[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x01, 0x00, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetDuplicationCheckError,24);
  testsOk &= (cama.setDuplicationCheck(false) == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_INVALID_DUP_VAL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetDuplicationCheck() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetDuplicationCheckTrueSuccess' ..."));
  const static PROGMEM uint8_t buf_GetDuplicationCheckTrueSuccess[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetDuplicationCheckTrueSuccess,24);
  testsOk &= (cama.getDuplicationCheck() == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetDuplicationCheckFalseSuccess' ..."));
  const static PROGMEM uint8_t buf_GetDuplicationCheckFalseSuccess[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetDuplicationCheckFalseSuccess,24);
  testsOk &= (cama.getDuplicationCheck() == false);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetDuplicationCheckError' ..."));
  const static PROGMEM uint8_t buf_GetDuplicationCheckError[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetDuplicationCheckError,24);
  testsOk &= (cama.getDuplicationCheck() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testStandbyMode() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'StandbyModeSuccess' ..."));
  const static PROGMEM uint8_t buf_StandbyModeSuccess[24] = {
    0xAA, 0x55, 0x17, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_StandbyModeSuccess,24);
  testsOk &= (cama.standbyMode() == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testSetSensorLed() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'SetSensorLedTrueSuccess' ..."));
  const static PROGMEM uint8_t buf_SetSensorLedTrueSuccess[24] = {
    0xAA, 0x55, 0x24, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_SetSensorLedTrueSuccess,24);
  testsOk &= (cama.setSensorLed(true) == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetEnrollCount() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetEnrollCountSuccess' ..."));
  const static PROGMEM uint8_t buf_GetEnrollCountSuccess[24] = {
    0xAA, 0x55, 0x28, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetEnrollCountSuccess,24);
  testsOk &= (cama.getEnrollCount() == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testGetEnrollCountTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'GetEnrollCountSuccessTimeout' ..."));
  const static PROGMEM uint8_t buf_GetEnrollCountSuccess[24] = {
    0xAA, 0x55, 0x28, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_GetEnrollCountSuccess,23);
  testsOk &= (cama.getEnrollCount() == 0);
  testsOk &= (cama.getLastResultCode() == 0xFFFF);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testFPCancel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'FPCancel' ..."));
  const static PROGMEM uint8_t buf_FPCancelSuccess[24] = {
    0xAA, 0x55, 0x30, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_FPCancelSuccess,24);
  cama.sendFPCancel();
  testsOk &= (cama.getFPCancelResult() == true);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

bool testIdentifyWithCancel() {
  bool testsOk=true;

  fpUserActionReturnValue = false;
  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'IdentifyWithCancel' ..."));
  const static PROGMEM uint8_t buf_IdentifyWithCancel[72] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x01, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x01,
    0xAA, 0x55, 0x30, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_IdentifyWithCancel,72);
  testsOk &= (cama.identify() == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FP_CANCEL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);
  fpUserActionReturnValue = true;

  return testsOk;
}

bool testTestConnection() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'TestConnectionSuccess' ..."));
  const static PROGMEM uint8_t buf_TestConnectionSuccess[24] = {
    0xAA, 0x55, 0x50, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_TestConnectionSuccess,24);
  testsOk &= (cama.testConnection(false) == 1);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println(F("RUN TEST 'TestConnectionFail' ..."));
  const static PROGMEM uint8_t buf_TestConnectionFail[24] = {
    0xAA, 0x55, 0x50, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x01
  };
  testStream.setBufferContentFromProgmem(buf_TestConnectionFail,24);
  testsOk &= (cama.testConnection(false) == 0);
  testsOk &= (cama.getLastResultCode() == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print(F("END TEST: testsOk = ")); Serial.println(testsOk);

  return testsOk;
}

void setup() {
  Serial.begin(9600);

  cama.setDebug(true); // so you can see the messages in the serial debug screen
  cama.setUserActionCallback(FPSUserAction);
  cama.setSerialTimeout(10000);

  bool testsOk=true;

  testsOk &= testVerify();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testIdentify();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testEnroll();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testEnrollOneTime();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testClearTemplate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testClearAllTemplate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetEmptyId();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetTemplateStatus();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetBrokenTemplate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetSecurityLevel();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetSecurityLevel();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetFingerTimeout();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetFingerTimeout();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetDeviceId();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetDeviceId();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetFirmwareVersion();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testFingerDetect();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetBaudrate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetDuplicationCheck();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetDuplicationCheck();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testStandbyMode();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testSetSensorLed();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetEnrollCount();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testFPCancel();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testIdentifyWithCancel();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testTestConnection();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testGetEnrollCountTimeout();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testReadTemplate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());
  testsOk &= testWriteTemplate();
  Serial.print(F("Free Memory: "));Serial.println(freeMemory());

  Serial.println();
  Serial.print(F("TEST SUCCESS: "));
  Serial.println(testsOk);
}


void loop() {
}

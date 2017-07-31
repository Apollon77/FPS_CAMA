#include <Arduino.h>

#include <FPS_CAMA.h>
#include <MemStream.h>

/*
 * TODO:
 * - Aufsplitten in mehrere CPP Files
 *
 */

byte streamBuffer[512];
MemStream testStream(streamBuffer,512,0,false);
FPS_CAMA cama(testStream);

bool fpUserActionReturnValue=true;
bool FPSUserAction(const Command_Packet::Commands::Commands_Enum command, const Response_Packet::ResultCodes::ResultCodes_Enum action) {
  Serial.println();
  Serial.print("Command=0x");
  Serial.print(command,HEX);
  Serial.print(", action=0x");
  Serial.print(action,HEX);
  switch(action) {
    case Response_Packet::ResultCodes::GD_NEED_RELEASE_FINGER: Serial.print(" - NEED RELEASE FINGER"); break; // Verify, Identify, Enroll
    case Response_Packet::ResultCodes::GD_NEED_FIRST_SWEEP: Serial.print(" - NEED FIRST SWEEP"); break; // Enroll
    case Response_Packet::ResultCodes::GD_NEED_SECOND_SWEEP: Serial.print(" - NEED SECOND SWEEP"); break; // Enroll
    case Response_Packet::ResultCodes::GD_NEED_THIRD_SWEEP: Serial.print(" - NEED THIRD SWEEP"); break; // Enroll
    default: Serial.println(" -- UNKNOWN ACTION!!!! -- ");
  }
  Serial.println();
  return fpUserActionReturnValue;
}

bool testVerify() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'VerifySuccess' ...");
  byte buf_VerifySuccess[48] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01
  };
  testStream.setBufferContent(buf_VerifySuccess,48);
  testsOk &= (cama.verify(1) == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'VerifyError' ...");
  byte buf_VerifyError[24] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x01
  };
  testStream.setBufferContent(buf_VerifyError,24);
  testsOk &= (cama.verify(1) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'VerifyErrorInvTmplId' ...");
  byte buf_VerifyErrorInvTmplId[48] = {
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01
  };
  testStream.setBufferContent(buf_VerifyErrorInvTmplId,48);
  testsOk &= (cama.verify(1) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testIdentify() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'IdentifySuccess' ...");
  byte buf_IdentifySuccess[48] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x01
  };
  testStream.setBufferContent(buf_IdentifySuccess,48);
  testsOk &= (cama.identify() == 2);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'IdentifyError' ...");
  byte buf_IdentifyError[24] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x01
  };
  testStream.setBufferContent(buf_IdentifyError,24);
  testsOk &= (cama.identify() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_ALL_TMPL_EMPTY);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testEnroll() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollSuccess' ...");
  byte buf_EnrollSuccess[336] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF2, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContent(buf_EnrollSuccess,336);
  testsOk &= (cama.enroll(3) == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollError' ...");
  byte buf_EnrollError[336] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF2, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x02,
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x01
  };
  testStream.setBufferContent(buf_EnrollError,336);
  testsOk &= (cama.enroll(3) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_GENERALIZE);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollErrorInvTmplId' ...");
  byte buf_EnrollErrorInvTmplId[24] = {
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x01
  };
  testStream.setBufferContent(buf_EnrollErrorInvTmplId,24);
  testsOk &= (cama.enroll(3) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testEnrollOneTime() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollOneTimeSuccess' ...");
  byte buf_EnrollOneTimeSuccess[48] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0x02,
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01
  };
  testStream.setBufferContent(buf_EnrollOneTimeSuccess,48);
  testsOk &= (cama.enrollOneTime(4) == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollOneTimeError' ...");
  byte buf_EnrollOneTimeError[24] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0x01
  };
  testStream.setBufferContent(buf_EnrollOneTimeError,24);
  testsOk &= (cama.enrollOneTime(4) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'EnrollOneTimeErrorInvTmplId' ...");
  byte buf_EnrollOneTimeErrorInvTmplId[48] = {
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0x02,
    0xAA, 0x55, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContent(buf_EnrollOneTimeErrorInvTmplId,48);
  testsOk &= (cama.enrollOneTime(4) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testClearTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'ClearTemplateSuccess' ...");
  byte buf_ClearTemplateSuccess[24] = {
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01
  };
  testStream.setBufferContent(buf_ClearTemplateSuccess,24);
  testsOk &= (cama.clearTemplate(1) == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'ClearTemplateError' ...");
  byte buf_ClearTemplateError[24] = {
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x01
  };
  testStream.setBufferContent(buf_ClearTemplateError,24);
  testsOk &= (cama.clearTemplate(1) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testClearAllTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'ClearAllTemplateSuccess' ...");
  byte buf_ClearAllTemplateSuccess[24] = {
    0xAA, 0x55, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01
  };
  testStream.setBufferContent(buf_ClearAllTemplateSuccess,24);
  testsOk &= (cama.clearAllTemplate() == 5);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'ClearAllTemplateError' ...");
  byte buf_ClearAllTemplateError[24] = {
    0xAA, 0x55, 0x06, 0x01, 0x04, 0x00, 0x01, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x01
  };
  testStream.setBufferContent(buf_ClearAllTemplateError,24);
  testsOk &= (cama.clearAllTemplate() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_EXCEPTION);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetEmptyId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetEmptyIdSuccess' ...");
  byte buf_GetEmptyIdSuccess[24] = {
    0xAA, 0x55, 0x07, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x01
  };
  testStream.setBufferContent(buf_GetEmptyIdSuccess,24);
  testsOk &= (cama.getEmptyId() == 2);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetEmptyIdError' ...");
  byte buf_GetEmptyIdError[24] = {
    0xAA, 0x55, 0x07, 0x01, 0x04, 0x00, 0x01, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x01
  };
  testStream.setBufferContent(buf_GetEmptyIdError,24);
  testsOk &= (cama.getEmptyId() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_EMPTY_ID_NOEXIST);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetTemplateStatus() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetTemplateStatusUsedSuccess' ...");
  byte buf_GetTemplateStatusUsedSuccess[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x01
  };
  testStream.setBufferContent(buf_GetTemplateStatusUsedSuccess,24);
  testsOk &= (cama.getTemplateStatus(1) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::GD_TEMPLATE_NOT_EMPTY);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetTemplateStatusFreeSuccess' ...");
  byte buf_GetTemplateStatusFreeSuccess[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01
  };
  testStream.setBufferContent(buf_GetTemplateStatusFreeSuccess,24);
  testsOk &= (cama.getTemplateStatus(1) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::GD_TEMPLATE_EMPTY);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetTemplateStatusError' ...");
  byte buf_GetTemplateStatusError[24] = {
    0xAA, 0x55, 0x08, 0x01, 0x04, 0x00, 0x01, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x01
  };
  testStream.setBufferContent(buf_GetTemplateStatusError,24);
  testsOk &= (cama.getTemplateStatus(1) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetBrokenTemplate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetBrokenTemplateSuccess' ...");
  byte buf_GetBrokenTemplateSuccess[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01
  };
  testStream.setBufferContent(buf_GetBrokenTemplateSuccess,24);
  testsOk &= (cama.getBrokenTemplate() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetBrokenTemplateSuccess2' ...");
  byte buf_GetBrokenTemplateSuccess2[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContent(buf_GetBrokenTemplateSuccess2,24);
  testsOk &= (cama.getBrokenTemplate() == 3);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetBrokenTemplateError' ...");
  byte buf_GetBrokenTemplateError[24] = {
    0xAA, 0x55, 0x09, 0x01, 0x06, 0x00, 0x01, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContent(buf_GetBrokenTemplateError,24);
  testsOk &= (cama.getBrokenTemplate() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_NOT_AUTHORIZED);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testSetSecurityLevel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetSecurityLevelSuccess' ...");
  byte buf_SetSecurityLevelSuccess[24] = {
    0xAA, 0x55, 0x0C, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x01
  };
  testStream.setBufferContent(buf_SetSecurityLevelSuccess,24);
  testsOk &= (cama.setSecurityLevel(3) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetSecurityLevelError' ...");
  byte buf_SetSecurityLevelError[24] = {
    0xAA, 0x55, 0x0C, 0x01, 0x04, 0x00, 0x01, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x01
  };
  testStream.setBufferContent(buf_SetSecurityLevelError,24);
  testsOk &= (cama.setSecurityLevel(6) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_SEC_VAL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetSecurityLevel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetSecurityLevelSuccess' ...");
  byte buf_GetSecurityLevelSuccess[24] = {
    0xAA, 0x55, 0x0D, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContent(buf_GetSecurityLevelSuccess,24);
  testsOk &= (cama.getSecurityLevel() == 3);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetSecurityLevelError' ...");
  byte buf_GetSecurityLevelError[24] = {
    0xAA, 0x55, 0x0D, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x01
  };
  testStream.setBufferContent(buf_GetSecurityLevelError,24);
  testsOk &= (cama.getSecurityLevel() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testSetFingerTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetFingerTimeoutSuccess' ...");
  byte buf_SetFingerTimeoutSuccess[24] = {
    0xAA, 0x55, 0x0E, 0x01, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x01
  };
  testStream.setBufferContent(buf_SetFingerTimeoutSuccess,24);
  testsOk &= (cama.setFingerTimeout(10) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetFingerTimeoutError' ...");
  byte buf_SetFingerTimeoutError[24] = {
    0xAA, 0x55, 0x0E, 0x01, 0x04, 0x00, 0x01, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x01
  };
  testStream.setBufferContent(buf_SetFingerTimeoutError,24);
  testsOk &= (cama.setFingerTimeout(61) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TIME_OUT);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetFingerTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetFingerTimeoutSuccess' ...");
  byte buf_GetFingerTimeoutSuccess[24] = {
    0xAA, 0x55, 0x0F, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01
  };
  testStream.setBufferContent(buf_GetFingerTimeoutSuccess,24);
  testsOk &= (cama.getFingerTimeout() == 5);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetFingerTimeoutError' ...");
  byte buf_GetFingerTimeoutError[24] = {
    0xAA, 0x55, 0x0F, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x01
  };
  testStream.setBufferContent(buf_GetFingerTimeoutError,24);
  testsOk &= (cama.getFingerTimeout() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testSetDeviceId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetDeviceIdSuccess' ...");
  byte buf_SetDeviceIdSuccess[24] = {
    0xAA, 0x55, 0x10, 0x01, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x01
  };
  testStream.setBufferContent(buf_SetDeviceIdSuccess,24);
  testsOk &= (cama.setDeviceId(10) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetDeviceIdError' ...");
  byte buf_SetDeviceIdError[24] = {
    0xAA, 0x55, 0x10, 0x01, 0x04, 0x00, 0x01, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x01
  };
  testStream.setBufferContent(buf_SetDeviceIdError,24);
  testsOk &= (cama.setDeviceId(266) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_PARAM);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetDeviceId() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetDeviceIdSuccess' ...");
  byte buf_GetDeviceIdSuccess[24] = {
    0xAA, 0x55, 0x11, 0x01, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContent(buf_GetDeviceIdSuccess,24);
  testsOk &= (cama.getDeviceId() == 5);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetDeviceIdError' ...");
  byte buf_GetDeviceIdError[24] = {
    0xAA, 0x55, 0x11, 0x01, 0x04, 0x00, 0x01, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x01
  };
  testStream.setBufferContent(buf_GetDeviceIdError,24);
  testsOk &= (cama.getDeviceId() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_EXCEPTION);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetFirmwareVersion() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetFirmwareVersionSuccess' ...");
  byte buf_GetFirmwareVersionSuccess[24] = {
    0xAA, 0x55, 0x12, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x01
  };
  testStream.setBufferContent(buf_GetFirmwareVersionSuccess,24);
  testsOk &= (cama.getFirmwareVersion() == 0x0902);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testFingerDetect() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'FingerDetectSuccessTrue' ...");
  byte buf_FingerDetectSuccessTrue[24] = {
    0xAA, 0x55, 0x13, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01
  };
  testStream.setBufferContent(buf_FingerDetectSuccessTrue,24);
  testsOk &= (cama.fingerDetect() == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'FingerDetectSuccessFalse' ...");
  byte buf_FingerDetectSuccessFalse[24] = {
    0xAA, 0x55, 0x13, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x01
  };
  testStream.setBufferContent(buf_FingerDetectSuccessFalse,24);
  testsOk &= (cama.fingerDetect() == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testSetBaudrate() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetBaudrateSuccess' ...");
  byte buf_SetBaudrateSuccess[24] = {
    0xAA, 0x55, 0x14, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x01
  };
  testStream.setBufferContent(buf_SetBaudrateSuccess,24);
  testsOk &= (cama.setBaudrate(1) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetBaudrateError' ...");
  byte buf_SetBaudrateError[24] = {
    0xAA, 0x55, 0x14, 0x01, 0x04, 0x00, 0x01, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x01
  };
  testStream.setBufferContent(buf_SetBaudrateError,24);
  testsOk &= (cama.setBaudrate(7) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_BAUDRATE);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testSetDuplicationCheck() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetDuplicationCheckSuccess' ...");
  byte buf_SetDuplicationCheckSuccess[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContent(buf_SetDuplicationCheckSuccess,24);
  testsOk &= (cama.setDuplicationCheck(true) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetDuplicationCheckSuccess2' ...");
  byte buf_SetDuplicationCheckSuccess2[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x01
  };
  testStream.setBufferContent(buf_SetDuplicationCheckSuccess2,24);
  testsOk &= (cama.setDuplicationCheck(false) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetDuplicationCheckError' ...");
  byte buf_SetDuplicationCheckError[24] = {
    0xAA, 0x55, 0x15, 0x01, 0x04, 0x00, 0x01, 0x00, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x01
  };
  testStream.setBufferContent(buf_SetDuplicationCheckError,24);
  testsOk &= (cama.setDuplicationCheck(false) == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_DUP_VAL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetDuplicationCheck() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetDuplicationCheckTrueSuccess' ...");
  byte buf_GetDuplicationCheckTrueSuccess[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01
  };
  testStream.setBufferContent(buf_GetDuplicationCheckTrueSuccess,24);
  testsOk &= (cama.getDuplicationCheck() == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetDuplicationCheckFalseSuccess' ...");
  byte buf_GetDuplicationCheckFalseSuccess[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x01
  };
  testStream.setBufferContent(buf_GetDuplicationCheckFalseSuccess,24);
  testsOk &= (cama.getDuplicationCheck() == false);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetDuplicationCheckError' ...");
  byte buf_GetDuplicationCheckError[24] = {
    0xAA, 0x55, 0x16, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01
  };
  testStream.setBufferContent(buf_GetDuplicationCheckError,24);
  testsOk &= (cama.getDuplicationCheck() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testStandbyMode() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'StandbyModeSuccess' ...");
  byte buf_StandbyModeSuccess[24] = {
    0xAA, 0x55, 0x17, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x01
  };
  testStream.setBufferContent(buf_StandbyModeSuccess,24);
  testsOk &= (cama.standbyMode() == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);
  
  return testsOk;
}

bool testSetSensorLed() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'SetSensorLedTrueSuccess' ...");
  byte buf_SetSensorLedTrueSuccess[24] = {
    0xAA, 0x55, 0x24, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01
  };
  testStream.setBufferContent(buf_SetSensorLedTrueSuccess,24);
  testsOk &= (cama.setSensorLed(true) == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetEnrollCount() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetEnrollCountSuccess' ...");
  byte buf_GetEnrollCountSuccess[24] = {
    0xAA, 0x55, 0x28, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x01
  };
  testStream.setBufferContent(buf_GetEnrollCountSuccess,24);
  testsOk &= (cama.getEnrollCount() == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testGetEnrollCountTimeout() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'GetEnrollCountSuccessTimeout' ...");
  byte buf_GetEnrollCountSuccess[24] = {
    0xAA, 0x55, 0x28, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x01
  };
  testStream.setBufferContent(buf_GetEnrollCountSuccess,23);
  testsOk &= (cama.getEnrollCount() == 0);
  testsOk &= (cama.lastResultCode == 0xFFFF);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testFPCancel() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'FPCancel' ...");
  byte buf_FPCancelSuccess[24] = {
    0xAA, 0x55, 0x30, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContent(buf_FPCancelSuccess,24);
  cama.sendFPCancel();
  testsOk &= (cama.getFPCancelResult() == true);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

bool testIdentifyWithCancel() {
  bool testsOk=true;

  fpUserActionReturnValue = false;
  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'IdentifyWithCancel' ...");
  byte buf_IdentifyWithCancel[72] = {
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x02,
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x01, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x01,
    0xAA, 0x55, 0x30, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01
  };
  testStream.setBufferContent(buf_IdentifyWithCancel,72);
  testsOk &= (cama.identify() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FP_CANCEL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);
  fpUserActionReturnValue = true;

  return testsOk;
}

bool testTestConnection() {
  bool testsOk=true;

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'TestConnectionSuccess' ...");
  byte buf_TestConnectionSuccess[24] = {
    0xAA, 0x55, 0x50, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x01
  };
  testStream.setBufferContent(buf_TestConnectionSuccess,24);
  testsOk &= (cama.testConnection() == 1);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'TestConnectionFail' ...");
  byte buf_TestConnectionFail[24] = {
    0xAA, 0x55, 0x50, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x01
  };
  testStream.setBufferContent(buf_TestConnectionFail,24);
  testsOk &= (cama.testConnection() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  return testsOk;
}

void setup() {
  Serial.begin(9600);

  cama.useSerialDebug=true;
  cama.setUserActionCallback(FPSUserAction);

  bool testsOk=true;

  testsOk &= testVerify();
  testsOk &= testIdentify();
  testsOk &= testEnroll();
  testsOk &= testEnrollOneTime();
  testsOk &= testClearTemplate();
  testsOk &= testClearAllTemplate();
  testsOk &= testGetEmptyId();
  testsOk &= testGetTemplateStatus();
  testsOk &= testGetBrokenTemplate();
  testsOk &= testSetSecurityLevel();
  testsOk &= testGetSecurityLevel();
  testsOk &= testSetFingerTimeout();
  testsOk &= testGetFingerTimeout();
  testsOk &= testSetDeviceId();
  testsOk &= testGetDeviceId();
  testsOk &= testGetFirmwareVersion();
  testsOk &= testFingerDetect();
  testsOk &= testSetBaudrate();
  testsOk &= testSetDuplicationCheck();
  testsOk &= testGetDuplicationCheck();
  testsOk &= testSetSensorLed();
  testsOk &= testGetEnrollCount();
  testsOk &= testFPCancel();
  testsOk &= testIdentifyWithCancel();
  testsOk &= testTestConnection();
  testsOk &= testGetEnrollCountTimeout();

  Serial.println();
  Serial.print("TEST SUCCESS: ");
  Serial.println(testsOk);
}


void loop() {
}
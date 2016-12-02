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
  }
  Serial.println();
  return true;
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

  Serial.println();
  Serial.print("TEST SUCCESS: ");
  Serial.println(testsOk);
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
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0xF4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x02,
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01
  };
  testStream.setBufferContent(buf_IdentifySuccess,48);
  testsOk &= (cama.identify() == 2);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_SUCCESS);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);

  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'IdentifyError' ...");
  byte buf_IdentifyError[24] = { 
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x01
  };
  testStream.setBufferContent(buf_IdentifyError,24);
  testsOk &= (cama.identify() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
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
    0xAA, 0x55, 0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01
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
    0xAA, 0x55, 0x03, 0x01, 0x04, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x01
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
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x01
  };
  testStream.setBufferContent(buf_ClearTemplateError,24);
  testsOk &= (cama.clearTemplate(1) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_INVALID_TMPL_NO);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);
  
  Serial.println(); Serial.println(); Serial.println();
  Serial.println("RUN TEST 'ClearTemplateErrorInvTmplId' ...");
  byte buf_ClearTemplateErrorInvTmplId[24] = { 
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x01
  };
  testStream.setBufferContent(buf_ClearTemplateErrorInvTmplId,24);
  testsOk &= (cama.clearTemplate(1) == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
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
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x01
  };
  testStream.setBufferContent(buf_ClearAllTemplateError,24);
  testsOk &= (cama.clearAllTemplate() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_FAIL);
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
    0xAA, 0x55, 0x05, 0x01, 0x04, 0x00, 0x01, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x01
  };
  testStream.setBufferContent(buf_GetEmptyIdError,24);
  testsOk &= (cama.getEmptyId() == 0);
  testsOk &= (cama.lastResultCode == Response_Packet::ResultCodes::ERR_EMPTY_ID_NOEXIST);
  Serial.print("END TEST: testsOk = "); Serial.println(testsOk);
  
  return testsOk;
}


void loop() {
}


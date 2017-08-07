# FPS_CAMA
Arduino Library for communication with Camabio Fingerprint sensors/modules (http://www.camabio.com/)

See folder "docs" for official firmware documentation.

## Functions
Implemented functions currently are:

* verify
* identify
* enroll
* enrollOneTime
* clearTemplate
* clearAllTemplate
* getEmptyId
* getTemplateStatus
* getBrokenTemplate
* setSecurityLevel
* getSecurityLevel
* setFingerTimeout
* getFingerTimeout
* setDeviceId
* getDeviceId
* getFWVersion
* fingerDetect
* setBaudRate
* setDuplicationCheck
* getDuplicationCheck
* enterStandbyMode
*  sensorLEDControl
* getEnrollCount
* FPCancel
* testConnection

## Examples

### FPS_CAMA_Tests
You need to have installed the Library "MemStream" from https://github.com/Apollon77/MemStream for this sketch to work, because this will be used as Serial simulator.

These are kind of unit tests for the library and will test all implemented functions if they work as expected without really connection to a Fingerprint device.
At the End it provides an Info if everything work as expected on your device before you connect your device to it. if the last line states

```
TEST SUCCESS: 1
```

Then everything is good. Else search the test with result 0 and check it or send it to me.

## TODO
* finally write README
* add Library to ArduinoIDE :-)

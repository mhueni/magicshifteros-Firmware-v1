#include "../firmware.h"

void CompassMode()
{
  int autoCalResetCounter = 0;
  
  int c = readRegister(0x0D);	// Read WHO_AM_I register
  Serial.print("Device: ");
  Serial.println(c, HEX);

  int frame = 0;

  v = 128;

  AccelConfig config;
  config.accelInterruptNr = 1;
  config.tapInterruptNr = 0;
  config.transientInterruptNr = 0;
  config.accelScale = 2;
  config.accelDataRate = 1;
  config.autoCalibrateMagnetometer = true;
  AccelSetup(&config);

  while (1) {
    frame++;
    AccelPoll();
                
    if (digitalRead(int2Pin) && (readRegister(0x5E) & 0x02)) {
      Serial.println("Magnetic Jamming detected");
      autoCalResetCounter++;
      if (autoCalResetCounter > 10) {
        Serial.println("Resetting Hard Iron Estimation...");
        // M_CTRL_REG2: Hybrid auto increment, Magnetic measurement min/max detection function reset
        writeRegister(0x5C, 0x24);
        autoCalResetCounter = 0;
      }
    }
    else {
      autoCalResetCounter = 0;
    }

    //    setAll(frame % 300 == 0 ? v : 0, frame % 300 == 100 ? v : 0, frame % 300 == 200 ? v : 0);
    //    setRGB(15, centerBtnPressed ? 100 : 0, powerBtnPressed ? 100 : 0, 0);
    //		updateLedsWithBlank();

    if (!MagicShifter_Poll()) break;
    
    int degrees = int(atan2(magnetValue[YAXIS], magnetValue[XAXIS]) * 180 / M_PI);
    setAll(0,0,0);
    int lednr = map(abs(degrees), 0, 180, 0, 15);
    setRGB(lednr, 0, 0, 100);
    Serial.print("LED: ");
    Serial.println(lednr);
    updateLedsWithBrightness();

    if (powerBtnClickedTime != 0) {
      //      setAll(90, 100, 12);
      //      updateLedsWithBlank();
      delay(10);
      powerBtnClickedTime = 0;

      Serial.print("Accel:");
      Serial.print(accelCount[XAXIS], DEC);
      Serial.print(" ");
      Serial.print(accelCount[YAXIS], DEC);
      Serial.print(" ");
      Serial.println(accelCount[ZAXIS], DEC);
      Serial.print("Magnet:");
      Serial.print(magnetValue[XAXIS], DEC);
      Serial.print(" ");
      Serial.print(magnetValue[YAXIS], DEC);
      Serial.print(" ");
      Serial.println(magnetValue[ZAXIS], DEC);

      Serial.print("Heading: ");
      Serial.println(degrees);
    }
    //    if (centerBtnClickedTime != 0) {
    //      setAll(23, 12, 100);
    //      updateLedsWithBlank();
    //      delay(10);
    //      centerBtnClickedTime = 0;
    //    }
  }
}

// Lillygo T-Display ESP32-S3
// PINOUT = https://cdn.cnx-software.com/wp-content/uploads/2022/07/T-Display-S3-pinout-diagram.jpg?lossy=0&strip=none&ssl=1
// 170 x 320 TFT     x,y = 0,0 is TOP LHS corner

/*Setup or a Touchpress or any error can Activate the display(DisplayActive true) and starts a millis time out interval - display is slept to stop pixel burn in and power save At a .5sec display update interval... is DisplayActive Timeout up ? if so we Dim and disable the Display

  is Displayactive? ... we will now show relevant screen
    
    An alarm warning Screen
    else which ever is the current active screen

*/

//Display & progress bar variables
int progress = 0;
int w = 120;
int h = 18;
int x = 12;
int y = 90;
int blocks = 0;



void UpdateDisplay(void) {

  //firstly , we chaeck if the display Active...a timeout is used to sleep the display preventing pixel burn-in and to save power

  if (DisplayActive && ((millis() - DisplayActivated_mS) >= LCD_On_Duration)) {
    Display_To_Sleep();
    DisplayActive = false;
  }

  if (DisplayActive) {  //OR there is some error to Display?

    if (millis() - LastDisplayChange_mS >= 20000) {  //After a 15sec timeout the display returns to the main screen
      DisplayID = 0;
    }

    //Which Display are we showing??......
    switch (DisplayID) {

      case 0:
        UpdateDisplay2();
        break;

      case 1:
        Display_Cells_1to8();
        break;

      case 2:
        Display_Cells_9to16();

        break;

      case 3:
        Display_All_Cells();

        break;

      case 4:
        CAN_Display();
        break;

      case 5:
        ExtraInfoScreen();
        break;
    }
  }
}

//================================================================================


void Display_To_Sleep(void) {

  for (int i = 254; i >= 0; i--) {  //255 = full brightness, 0 = Backlight Off
    //ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, i);  // PWM steadily reduced to dim the backlight
    delay(10);
  }

  lcd.writecommand(ST7789_DISPOFF);  // Switch off the display
  lcd.writecommand(ST7789_SLPIN);    // Sleep the display driver
  Serial.println("Display BL OFF and Disabled");
}

//==================================================================================
void Activate_display(void) {       //Used to wake the display after it has timed out and gone blank..

  Serial.println("activated display");
  lcd.init();
  lcd.fillScreen(TFT_BLACK);
  lcd.setRotation(1);  //Display is rotated right to landscape
  sprite.createSprite(320, 170);
  //sprite.setTextDatum(3);
  sprite.setSwapBytes(true);

  DisplayActivated_mS = millis();

  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, 255);
}

//==================================================================================

void BootSplashScreen(void) {

  sprite.fillSprite(grey);  //TFT_WHITE);
  sprite.pushImage(0, 0, 320, 170, BMS_and_Inverter_Splash);
  sprite.setTextColor(TFT_WHITE);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.drawString("JK-BMS to CAN ", 50, 8);


  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Steve Tearle 2023 Firmware Ver ", 30, 33, 2);
  sprite.drawString(SW_Version, 240, 32, 2);

  sprite.pushSprite(0, 0);
}

//===================================================================================


void ScanScreen(void) {  //  Welcome screen - Shows BMS to Pylontech, Firmware, Scanning message and CAN status

  lcd.fillScreen(TFT_BLACK);  // set background black
  lcd.setSwapBytes(true);
  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  lcd.drawString("JK BMS to Pylontech LV", 20, 20, 4);

  
  lcd.setTextColor(TFT_WHITE, TFT_GREEN);

  // sprite.fillRoundRect(5, 62, 315, 60, 4, orange);  // fillRoundRect (x0, y0, w, h, radius, color)

  lcd.setFreeFont(&Orbitron_Light_24);

  if (!CAN_OK) {
    // lcd.drawRoundRect(5, 40, 180,80,4,TFT_RED);  //outline rectangle only
    lcd.fillRoundRect(40, 50, 250, 45, 4, TFT_WHITE);
    lcd.setTextColor(TFT_RED);
    lcd.drawString("CAN Driver Failed !", 50, 75, 4);
    delay(10000);
    ESP.restart();  //TWAI / CAN driver failed to load - Nothing we can do! Re-Boot
  }

  lcd.drawRoundRect(5, 40, 180, 80, 4, TFT_RED);
  lcd.setFreeFont(&Orbitron_Light_24);
  
  lcd.setTextColor(TFT_RED, TFT_BLACK);
  lcd.drawString("Scanning", 30, 60);
  lcd.drawString("For BMS", 30, 90);

  lcd.pushImage(220, 45, 75, 75, BLE_Signal_Grey);
  
}

//================================================================================

void DisplayBMSFound(void) {  //  Welcome screen - Shows BMS to Pylontech, Firmware, Scanning message and CAN status

  //sprite.fillRoundRect(5, 62, 315, 60, 4, TFT_BLACK);  // fillRoundRect (x0, y0, w, h, radius, color)
  sprite.drawRoundRect(5, 62, 315, 60, 4, TFT_GREEN);
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);

  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.drawString("      JK BMS Found!     ", 20, 90);
  sprite.pushSprite(0, 0);
  delay(1000);
}

//================================================================================

void ScanScreenLayout(void) {  //Shows scanning for BMS, software version Header,

  sprite.fillSprite(TFT_BLACK);  // set background black
  // sprite.setTextDatum(4);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString(SW_Version, 20, 20, 2);
  sprite.drawString("Steve Tearle 2023", 20, 40, 2);

  sprite.setTextColor(TFT_WHITE, TFT_BLUE);
  sprite.fillRoundRect(5, 62, 315, 60, 4, purple);  // fillRoundRect (x0, y0, w, h, radius, color)
  //sprite.fillRoundRect(6,58,80,18,4,green);

  sprite.setTextColor(TFT_WHITE, purple);
  sprite.drawString(" Scanning for JKBMS...", 20, 92, 4);

  sprite.pushSprite(0, 0);
}



//-------------------------------------------------------------------------------
void RestartingScreen(void) {  //Shows scanning for BMS, software version Header,
                               //advertising server device names shown (from in Advertising callback

  sprite.fillSprite(TFT_BLACK);  // set background black
  // sprite.setTextDatum(4);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Something has gone wrong!", 20, 20, 2);
  sprite.drawString("", 20, 40, 2);

  sprite.setTextColor(TFT_WHITE, TFT_BLUE);
  sprite.fillRoundRect(5, 62, 315, 60, 4, purple);  // fillRoundRect (x0, y0, w, h, radius, color)
  //sprite.fillRoundRect(6,58,80,18,4,green);

  sprite.setTextColor(TFT_WHITE, purple);
  sprite.drawString(" RE-INITIALISING !...", 20, 92, 4);

  sprite.pushSprite(0, 0);
}


//*****************************************************************************8


//---------------------------------------------------------------------------------------------

void UpdateDisplay2() {  //The main data screen, Batt V , I, SOC, delta V, Temperature

  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);  //pin 38 controlls Backlight on T-Display S3, High is 255 = full On,  This pin is on Channel 0
  ledcWrite(0, 254);

  sprite.fillSprite(TFT_BLACK);  //add extra fonts from Lib
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("Charge:", 8, 12);
  sprite.setFreeFont(&Orbitron_Light_32);

  sprite.drawString(String(SOC_Percent) + "%", 18, 45);

  // Draw progress bar, block colour depends on value...
  w = 120;
  h = 20;
  x = 8;
  y = 75;

  blocks = SOC_Percent / 5;

  sprite.drawRoundRect(x, y, w, h, 3, TFT_GREEN);

  if (SOC_Percent > 80) {  //we draw progress bar using different colours for different SOCs
    for (int i = 0; i < blocks; i++)
      sprite.fillRect(i * 5 + (x + 3) + (i * 1), y + 4, 5, 11, TFT_GREEN);
  } else if (SOC_Percent > 55) {
    for (int i = 0; i < blocks; i++)
      sprite.fillRect(i * 5 + (x + 3) + (i * 1), y + 4, 5, 11, TFT_YELLOW);
  } else {
    for (int i = 0; i < blocks; i++)
      sprite.fillRect(i * 5 + (x + 3) + (i * 1), y + 4, 5, 11, TFT_RED);
  }

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("CELL VOLTAGES:-", 10, 110, 2);

  sprite.setTextColor(TFT_RED, TFT_BLACK);
  sprite.drawString("Ave: " + String(Average_Cell_Voltage, 3) + "V", 4, 135, 4);

  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  sprite.drawString("  dV: " + String(Delta_Cell_Voltage, 3) + "V", 4, 160, 4);

  sprite.setTextFont(0);

  // delay(6);

  //Large Volts
  sprite.setFreeFont();
  sprite.setTextColor(TFT_BLUE, TFT_BLACK);
  // sprite.drawString("Volts ",110,35,2);
  sprite.drawString(String(Battery_Voltage, 2), 155, 25, 7);  ///..String function converts Float to string with 3 dec places
  sprite.drawString("V", 300, 37, 4);                         ///..String function converts Float to string with 3 dec places

  //Large Current
  if (Charge_Current >= 0) {
    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  } else {
    sprite.setTextColor(TFT_RED, TFT_BLACK);
  }

  sprite.drawString(String(Charge_Current, 1), 150, 80, 7);  ///..String function converts Float to string with 3 dec places
  sprite.drawString("A", 300, 96, 4);                        ///..String function converts Float to string with 3 dec places



  // Display BMS Charge and Discharge switch Status
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("CHARGE", 147, 127, 2);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("DISCHARGE", 147, 157, 2);

  //charge = "off";     //for testing screen layout only!

  if (charge == "on") {
    sprite.fillRoundRect(220, 117, 30, 20, 4, TFT_GREEN);
    sprite.setTextColor(TFT_BLACK, TFT_GREEN);
    sprite.drawString("ON", 226, 127, 2);  ///..String function converts Float to string with 3 dec places
  } else {
    sprite.fillRoundRect(220, 117, 30, 20, 4, TFT_RED);
    sprite.setTextColor(TFT_WHITE, TFT_RED);
    sprite.drawString("OFF", 223, 127, 2);  ///..String function converts Float to string with 3 dec places
  }


  if (discharge == "on") {
    sprite.fillRoundRect(220, 147, 30, 20, 4, TFT_GREEN);
    sprite.setTextColor(TFT_BLACK, TFT_GREEN);
    sprite.drawString("ON", 226, 157, 2);  ///..String function converts Float to string with 3 dec places
  } else {
    sprite.fillRoundRect(220, 147, 30, 20, 4, TFT_RED);
    sprite.setTextColor(TFT_WHITE, TFT_RED);
    sprite.drawString("OFF", 223, 157, 2);  ///..String function converts Float to string with 3 dec places
  }

  if (LowTemp || HighTemp || CellOverVoltage || CellUnderVoltage) {
    sprite.setTextColor(TFT_YELLOW, TFT_BLACK);  //If ther is any temperature or voltage conditionind
    sprite.drawString("ALARM", 278, 163, 2);
    sprite.pushImage(277, 110, 44, 40, Warningtriangle3);
  } else {
    sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite.drawString("T1", 257, 127, 2);
    sprite.drawString("T2", 257, 157, 2);
    sprite.drawString("C", 310, 127, 2);
    sprite.drawString("C", 310, 157, 2);

    sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    sprite.drawString(String(Battery_T1, 1), 278, 127, 2);  //..String function converts Float to string with 3 dec places
    sprite.drawString(String(Battery_T2, 1), 278, 157, 2);
  }


  sprite.pushSprite(0, 0);
}


//*******************************************************************************************8

//***********************************************************
void Display_Cells_1to8(void) {  //Pressing RHS button shows this individual cell Voltage display

  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Cell Voltages (1-8):", 8, 16);

  /*int Cell_MaxV = cellVoltage[0];
  int Cell_MinV = cellVoltage[0];

  for (int i = 0; i < cell_count; i++) {
    Cell_MaxV = max(cellVoltage[i], Cell_MaxV);
    Cell_MinV = min(cellVoltage[i], Cell_MinV);
  }
*/


  int y = 55;
  int x = 4;
  int stepsize = 30;

  // Serial.print("The maximum value is: "); Serial.println(Cell_MaxV);
  // Serial.print("The minimum value is: "); Serial.println(Cell_MinV);

  //Now We print the voltage of Cells 1 to 8 to Display
  for (int i = 0; i < 8; i++) {

    if (Cell_MaxV == Cell_MinV) {  //Voltage values are colour coded....
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    } else if (cellVoltage[i] == Cell_MinV) {
      sprite.setTextColor(TFT_BLUE, TFT_BLACK);
    } else if (cellVoltage[i] == Cell_MaxV) {
      sprite.setTextColor(TFT_RED, TFT_BLACK);
    } else {
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    if (i < 4) {  //Print individual Cell Voltges to Display
      sprite.drawString("C" + String(i + 1) + ":  " + String(float(cellVoltage[i]) / 1000.0, 3) + "V", 4, (y + stepsize * i), 4);
    } else {  //Move printing across to 2nd Column
      sprite.drawString("C" + String(i + 1) + ":  " + String(float(cellVoltage[i]) / 1000.0, 3) + "V", 162, (y + stepsize * (i - 4)), 4);
    }
  }
  sprite.pushSprite(0, 0);
}
//****************************************************************************************************
void Display_Cells_9to16(void) {  //Pressing RHS button shows this individual cell Voltage display

  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Cell Voltages (9-16):", 8, 16);

  int y = 55;
  int x = 4;
  int stepsize = 30;

  // Serial.print("The maximum value is: "); Serial.println(Cell_MaxV);
  // Serial.print("The minimum value is: "); Serial.println(Cell_MinV);

  //Lets print each cell voltage to the Display...
  for (int i = 0; i < 8; i++) {  //

    if (Cell_MaxV == Cell_MinV) {  //Voltage values are colour coded....
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    } else if (cellVoltage[i + 8] == Cell_MinV) {  //we are showing cells 8 to 15 (16cells indexed 0-15 )
      sprite.setTextColor(TFT_BLUE, TFT_BLACK);
    } else if (cellVoltage[i + 8] == Cell_MaxV) {
      sprite.setTextColor(TFT_RED, TFT_BLACK);
    } else {
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    if (i < 4) {  //Print individual Cell Voltages,  first 4 cells positioned in left column, as this is cells 8 to 15 we have i+8 for cell position
      sprite.drawString("C" + String(i + 9) + ":  " + String(float(cellVoltage[i + 8]) / 1000.0, 3) + "V", 4, (y + stepsize * i), 4);
    } else {  //Move printing across to 2nd Column
      sprite.drawString("C" + String(i + 9) + ":  " + String(float(cellVoltage[i + 8]) / 1000.0, 3) + "V", 162, (y + stepsize * (i - 4)), 4);
    }
  }
  sprite.pushSprite(0, 0);
}


//****************************************************************************************************
void Display_All_Cells(void) {  //Pressing RHS button shows this individual cell Voltage display

  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Cell Volts:", 8, 16);

  int Cell_MaxV = cellVoltage[0];
  int Cell_MinV = cellVoltage[0];

  for (int i = 0; i < cell_count; i++) {
    Cell_MaxV = max(cellVoltage[i], Cell_MaxV);
    Cell_MinV = min(cellVoltage[i], Cell_MinV);
  }
  // Serial.print("The maximum value is: "); Serial.println(Cell_MaxV);
  // Serial.print("The minimum value is: "); Serial.println(Cell_MinV);


  int y = 50;         // vertical starting point for each column..50 pixels down from top
  int x = 5;          // x position of first column
  int stepsize = 20;  //Pixel step for each printed row


  //Now We print each cell voltage to Display
  for (int i = 0; i < cell_count; i++) {

    if (Cell_MaxV == Cell_MinV) {  //Voltage values are colour coded....
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    } else if (cellVoltage[i] == Cell_MinV) {
      sprite.setTextColor(TFT_BLUE, TFT_BLACK);
    } else if (cellVoltage[i] == Cell_MaxV) {
      sprite.setTextColor(TFT_RED, TFT_BLACK);
    } else {
      sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    if (i < 5) {  //Print 1st column of individual Cell Voltages
      sprite.drawString("C" + String(i + 1) + ":  " + String(float(cellVoltage[i]) / 1000.0, 3) + "V", 4, (y + stepsize * i), 2);
    } else if (i < 10) {  //Move printing across to 2nd Column
      sprite.drawString("C" + String(i + 1) + ":  " + String(float(cellVoltage[i]) / 1000.0, 3) + "V", 115, (y + stepsize * (i - 5)), 2);
    } else if (i < 16) {  //Move printing across to 2nd Column
      sprite.drawString("C" + String(i + 1) + ":  " + String(float(cellVoltage[i]) / 1000.0, 3) + "V", 220, (y + stepsize * (i - 10)), 2);
    }
  }

  // Show Cell delta V in Top RHS corner
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  sprite.drawString("dV= " + String(Delta_Cell_Voltage, 3) + "V", 180, 22, 4);
  sprite.pushSprite(0, 0);
}


//***********************************************************
void CAN_Display(void) {  //Pressing RHS button shows this individual cell Voltage display

  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_CYAN, TFT_BLACK);
  sprite.drawString("CAN PYLONTECH LV", 8, 16);


  //sprite.drawString("dV= " + String(Delta_Cell_Voltage, 3) + "V", 180, 22,4);

  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Max Charge Volts", 2, 55, 2);
  sprite.drawString(String(float(CAN_Battery_charge_voltage) / 10.0, 1) + "V", 155, 55, 2);

  sprite.drawString("Min Discharge Volts", 2, 75, 2);
  sprite.drawString(String(float(CAN_Battery_discharge_voltage) / 10.0, 1) + "V", 155, 75, 2);


  //Show the max charging current...this can be the default value or a reduced value due to cell SOC or dV etc
  if (CAN_Adjusted_Battery_charge_current_limit != CAN_Battery_charge_current_limit) {

    sprite.setTextColor(TFT_RED, TFT_BLACK);
    sprite.drawString("Adjusted Max Current", 2, 95, 2);
    sprite.drawString(String(float(CAN_Adjusted_Battery_charge_current_limit) / 10.0, 1) + "A", 155, 95, 2);

    sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite.drawString("(Default " + String(float(CAN_Battery_charge_current_limit / 10.0), 1) + "A)", 200, 95, 2);
  } else {
    sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite.drawString("Max Charge Current ", 2, 95, 2);
    sprite.drawString(String(float(CAN_Battery_charge_current_limit / 10.0), 1) + "A", 155, 95, 2);
  }


  sprite.drawString("Max Discharge Current", 2, 115, 2);
  sprite.drawString(String(float(CAN_Battery_discharge_current_limit) / 10.0, 1) + "A", 155, 115, 2);



  //bool LowTemp = false;  //  Alarm / Error  flags --set / clear to test display layout only!
  //bool HighTemp = false;
  //bool CellOverVoltage = false;
  //bool CellUnderVoltage = false;

  sprite.drawString("Alarm Flags : ", 2, 135, 2);

  sprite.setTextColor(TFT_RED, TFT_BLACK);
  if (LowTemp) {
    sprite.drawString("Low Temp ! ", 155, 135, 2);
  } else if (HighTemp) {
    sprite.drawString("High Temp ! ", 155, 135, 2);
  } else if (CellOverVoltage) {
    sprite.drawString("Over Voltage !", 155, 135, 2);
  } else if (CellUnderVoltage) {
    sprite.drawString("Under Voltage !", 155, 135, 2);
  } else {
    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    sprite.drawString("All OK ! ", 155, 135, 2);
  }

  sprite.setTextColor(TFT_RED, TFT_BLACK);
  if (LowTemp || HighTemp) {  //one of these was shown on first line, second line will show any Voltage alarm state
    if (CellOverVoltage) {
      sprite.drawString("Over Voltage !", 155, 155, 2);
    } else if (CellUnderVoltage) {
      sprite.drawString("Under Voltage !", 155, 155, 2);
    }
  }


  
  sprite.pushSprite(0, 0);
}



//**********************************************************************************************



void ExtraInfoScreen() {  // A screen for showing some extra BMS info ....

  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);  //pin 38 controlls Backlight on T-Display S3, High is 255 = full On,  This pin is on Channel 0
  ledcWrite(0, 254);


  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&Orbitron_Light_24);
  sprite.setTextColor(TFT_CYAN, TFT_BLACK);
  sprite.drawString("BMS Status Info ", 8, 10);


  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("Temp 1 ", 0, 43);

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(String(Battery_T1, 1), 190, 43);

  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("C", 285, 43);
  sprite.drawCircle(278, 41, 3, TFT_YELLOW);

  sprite.drawString("Temp 2 ", 0, 70);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(String(Battery_T2, 1), 190, 70);

  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString("C", 285, 70);
  sprite.drawCircle(278, 68, 3, TFT_YELLOW);

  

  sprite.drawString("Bal. Current", 0, 97);

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(String(Balance_Curr, 3), 190, 97);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);

  sprite.drawString("A", 285, 97);

  sprite.drawString("Alarms:", 0, 124);


  // Override status of OTP and UVP etc alarm flags
  // bool LowTemp = true;  //  Alarm / Error  flags --set / clear to test display layout only!
  // bool HighTemp = false;
  // bool CellOverVoltage = false;
  // bool CellUnderVoltage = true;

  sprite.setTextColor(TFT_RED, TFT_BLACK);

  if (LowTemp) {
    sprite.drawString("Low Temp! ", 112, 124);
  } else if (HighTemp) {
    sprite.drawString("High Temp! ", 112, 124);
  } else if (CellOverVoltage) {
    sprite.drawString("Over Voltage!", 112, 124);
  } else if (CellUnderVoltage) {
    sprite.drawString("Under Voltage!", 112, 124);
  } else {
    sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    sprite.drawString("All OK ! ", 190, 124);
  }

  sprite.setTextColor(TFT_RED, TFT_BLACK);

  if (LowTemp || HighTemp) {  //one of these was shown on first line, second line will show any Voltage alarm state
    if (CellOverVoltage) {
      sprite.drawString("Over Voltage!", 112, 152);
    } else if (CellUnderVoltage) {
      sprite.drawString("Under Voltage!", 112, 152);
    }
  }

  //Large Volts
  sprite.setFreeFont();
  sprite.setTextColor(TFT_BLUE, TFT_BLACK);
  // sprite.drawString("Volts ",110,35,2);

  sprite.pushSprite(0, 0);
}


//*******************************************************************************************

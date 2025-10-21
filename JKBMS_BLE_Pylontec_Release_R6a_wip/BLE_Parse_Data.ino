//FYI...this  EspHome code outlines data frame protocol https://github.com/syssi/esphome-jk-bms/blob/main/components/jk_bms_ble/jk_bms_ble.cpp

void ParseData() {

  uint8_t j, i;
  if (BMS_Version == 11) {
    //Cell voltages  Zellspannungen
    for (j = 0, i = 7; i < 70; j++, i += 2) {
      cellVoltage[j] = (((int)receivedBytes_main[i] << 8 | receivedBytes_main[i - 1]));
      //cellVoltage[j] = cellVoltage[j] + 0.0005;       // so that it rounds "correctly", to 3 decimal places 0.0005 usw.
      //cellVoltage[j] = (int)(cellVoltage[j] * 1000);  // Here the float *100 is calculated and cast to int, so all other decimal places are omitted
      //cellVoltage[j] = cellVoltage[j] / 1000;
    }

    // +20 byte Zelle 15 - 24
    // +4 Byte unbekannt

    Average_Cell_Voltage = (((int)receivedBytes_main[75] << 8 | receivedBytes_main[74]) * 0.001);
    Delta_Cell_Voltage = (((int)receivedBytes_main[77] << 8 | receivedBytes_main[76]) * 0.001);
    Current_Balancer = receivedBytes_main[78];

    // +48 byte Resistance_Cell1
    for (j = 0, i = 81; i < (81 + 48); j++, i += 2) {
      balanceWireResist[j] = (((int)receivedBytes_main[i] << 8 | receivedBytes_main[i - 1]));
    }


    Battery_Voltage = (((int)receivedBytes_main[153] << 24 | receivedBytes_main[152] << 16 | receivedBytes_main[151] << 8 | receivedBytes_main[150]) * 0.001);
    CAN_Battery_Voltage = (((int)receivedBytes_main[151] << 8 | receivedBytes_main[150]) / 10);
    Battery_Power = (((int)receivedBytes_main[157] << 24 | (int)receivedBytes_main[156] << 16 | (int)receivedBytes_main[155] << 8 | (int)receivedBytes_main[154]) * 0.001);
    Charge_Current = (((int)receivedBytes_main[161] << 24 | receivedBytes_main[160] << 16 | receivedBytes_main[159] << 8 | receivedBytes_main[158]) * 0.001);

    CAN_Battery_Current = (int)(Charge_Current * 10);

    if (receivedBytes_main[163] == 0xFF) {
      Battery_T1 = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[163] << 8 | (int)receivedBytes_main[162]) * 0.1);
      CAN_Battery_T1 = (0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[163] << 8 | receivedBytes_main[162]);
    } else {
      Battery_T1 = (((int)receivedBytes_main[163] << 8 | (int)receivedBytes_main[162]) * 0.1);
      CAN_Battery_T1 = ((int)receivedBytes_main[163] << 8 | receivedBytes_main[162]);
    }


    if (receivedBytes_main[133] == 0xFF) {
      Battery_T2 = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[165] << 8 | (int)receivedBytes_main[164]) * 0.1);
    } else {
      Battery_T2 = (((int)receivedBytes_main[165] << 8 | (int)receivedBytes_main[164]) * 0.1);
    }

    if (receivedBytes_main[145] == 0xFF) {
      MOS_Temp = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[15] << 8 | (int)receivedBytes_main[144]) * 0.1);
    } else {
      MOS_Temp = (((int)receivedBytes_main[145] << 8 | (int)receivedBytes_main[144]) * 0.1);
    }


    if ((receivedBytes_main[171] & 0xF0) == 0x0) {
      Balance_Curr = (((int)receivedBytes_main[171] << 8 | receivedBytes_main[170]) * 0.001);
    } else if ((receivedBytes_main[171] & 0xF0) == 0xF0) {
      Balance_Curr = ((((int)receivedBytes_main[171] & 0x0F) << 8 | receivedBytes_main[170]) * -0.001) + 2.000;
    }
    // +2 byte unbekant 4
    SOC_Percent = ((int)receivedBytes_main[173]);
    Capacity_Remain = (((int)receivedBytes_main[177] << 24 | receivedBytes_main[176] << 16 | receivedBytes_main[175] << 8 | receivedBytes_main[174]) * 0.001);
    Nominal_Capacity = (((int)receivedBytes_main[181] << 24 | receivedBytes_main[180] << 16 | receivedBytes_main[179] << 8 | receivedBytes_main[178]) * 0.001);
    Cycle_Count = receivedBytes_main[185] + receivedBytes_main[184] + receivedBytes_main[183] + receivedBytes_main[182];

    // +6 byte unbekant 5
    Capacity_Cycle = (((int)receivedBytes_main[193] << 8 | receivedBytes_main[192]) * 0.001);
    Uptime = (((int)receivedBytes_main[196] << 16 | receivedBytes_main[195] << 8 | receivedBytes_main[194]));
    sec = Uptime % 60;
    Uptime /= 60;
    mi = Uptime % 60;
    Uptime /= 60;
    hr = Uptime % 24;
    days = Uptime /= 24;

    // +1 byte unknown  unbekannt
    if (receivedBytes_main[198] > 0) {
      charge = "on";
    } else if (receivedBytes_main[198] == 0) {
      charge = "off";
    }
    if (receivedBytes_main[199] > 0) {
      discharge = "on";
    } else if (receivedBytes_main[199] == 0) {
      discharge = "off";
    }
  }
  //____________________Decoding for BMS Ver 10___________________________________________________

  if (BMS_Version == 10) {
    // Cell voltages
    for (j = 0, i = 7; i < 38; j++, i += 2) {  //stores int versions of the cell voltages x 1000

      cellVoltage[j] = (((int)receivedBytes_main[i] << 8 | receivedBytes_main[i - 1]));
    }



    // +20 byte Zelle 15 - 24
    // +4 Byte unknown unbekannt
    Average_Cell_Voltage = (((int)receivedBytes_main[59] << 8 | receivedBytes_main[58]) * 0.001);
    Delta_Cell_Voltage = (((int)receivedBytes_main[61] << 8 | receivedBytes_main[60]) * 0.001);
    //Serial.printf("Cell Delta V: %.3fV  \n",Delta_Cell_Voltage);

    Current_Balancer = (((int)receivedBytes_main[63] << 8 | receivedBytes_main[62]) * 0.001);

    // Serial.print("Current_Balancer   ");  // ...No of cells being balanced???
    // Serial.println(Current_Balancer);

    // +48 byte Resistance_Cell 1
    // +6 byte unknown 2
    Battery_Voltage = (((int)receivedBytes_main[121] << 24 | receivedBytes_main[120] << 16 | receivedBytes_main[119] << 8 | receivedBytes_main[118]) * 0.001);
    CAN_Battery_Voltage = (((int)receivedBytes_main[119] << 8 | receivedBytes_main[118]) / 10);


    Battery_Power = (((int)receivedBytes_main[125] << 24 | (int)receivedBytes_main[124] << 16 | (int)receivedBytes_main[123] << 8 | (int)receivedBytes_main[122]) * 0.001);
    Charge_Current = (((int)receivedBytes_main[129] << 24 | receivedBytes_main[128] << 16 | receivedBytes_main[127] << 8 | receivedBytes_main[126]) * 0.001);
    //CAN_Battery_Current = ((int)receivedBytes_main[129] << 24 | receivedBytes_main[128] << 16 | receivedBytes_main[127] << 8 | receivedBytes_main[126] / 100);
    CAN_Battery_Current = (int)(Charge_Current * 10);

    //Battery_Power = (((int)receivedBytes_main[121] << 8 | receivedBytes_main[120])*0.001); unbekannt ?!
    //Battery_Power = (((int)receivedBytes_main[123] << 8 | receivedBytes_main[122])*0.001);
    // +6 byte unbekant 3


    //Battery_T1 = (((int)receivedBytes_main[131] << 8 | receivedBytes_main[130])*0.1);
    //CAN_Battery_T1 = ((int)receivedBytes_main[131] << 8 | receivedBytes_main[130]);
    if (receivedBytes_main[131] == 0xFF) {
      Battery_T1 = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[131] << 8 | (int)receivedBytes_main[130]) * 0.1);
      CAN_Battery_T1 = (0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[131] << 8 | receivedBytes_main[130]);
    } else {
      Battery_T1 = (((int)receivedBytes_main[131] << 8 | (int)receivedBytes_main[130]) * 0.1);
      CAN_Battery_T1 = ((int)receivedBytes_main[131] << 8 | receivedBytes_main[130]);
    }


    //Battery_T2 = (((int)receivedBytes_main[133] << 8 | (int)receivedBytes_main[132])*0.1);
    if (receivedBytes_main[133] == 0xFF) {
      Battery_T2 = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[133] << 8 | (int)receivedBytes_main[132]) * 0.1);
    } else {
      Battery_T2 = (((int)receivedBytes_main[133] << 8 | (int)receivedBytes_main[132]) * 0.1);
    }


    //MOS_Temp = (((int)receivedBytes_main[135] << 8 | receivedBytes_main[134])*0.1);
    if (receivedBytes_main[135] == 0xFF) {
      MOS_Temp = ((0xFF << 24 | 0xFF << 16 | (int)receivedBytes_main[135] << 8 | (int)receivedBytes_main[134]) * 0.1);
    } else {
      MOS_Temp = (((int)receivedBytes_main[135] << 8 | (int)receivedBytes_main[134]) * 0.1);
    }


    // 137   2   0x00 0x00              System alarms
    //           0x00 0x01  bit 0         Charge overtemperature               0000 0000 0000 0001
    //           0x00 0x02                Charge undertemperature              0000 0000 0000 0010
    //           0x00 0x04                                                     0000 0000 0000 0100
    //           0x00 0x08                Cell Undervoltage                    0000 0000 0000 1000
    //           0x00 0x10                                                     0000 0000 0001 0000
    //           0x00 0x20                                                     0000 0000 0010 0000
    //           0x00 0x40                                                     0000 0000 0100 0000
    //           0x00 0x80  bit7                                               0000 0000 1000 0000
    //           0x01 0x00  bit0 (137)                                         0000 0001 0000 0000
    //           0x02 0x00                                                     0000 0010 0000 0000
    //           0x04 0x00                Cell count is not equal to settings  0000 0100 0000 0000
    //           0x08 0x00                Current sensor anomaly               0000 1000 0000 0000
    //           0x10 0x00                Cell Over Voltage                    0001 0000 0000 0000
    //           0x20 0x00                                                     0010 0000 0000 0000
    //           0x40 0x00                                                     0100 0000 0000 0000
    //           0x80 0x00                                                     1000 0000 0000 0000
    //
    //           0x14 0x00                Cell Over Voltage +                  0001 0100 0000 0000
    //                                    Cell count is not equal to settings
    //           0x04 0x08                Cell Undervoltage +                  0000 0100 0000 1000
    //                                    Cell count is not equal to settings



    LowTemp = bitRead(receivedBytes_main[137], 1);  //to read third bit of a byte variable use bitRead(input, 2)
    HighTemp = bitRead(receivedBytes_main[137], 0);
    CellUnderVoltage = bitRead(receivedBytes_main[137], 3);
    CellOverVoltage = bitRead(receivedBytes_main[136], 4);


    /* Serial.printf("Lowtemp %s \n", LowTemp ? "true" : "false");   // umm something to learn ......? "true" : "false"
      Serial.printf("Hightemp %s \n", HighTemp ? "true" : "false");
      Serial.printf("CellUnderVoltage %s \n", CellUnderVoltage ? "true" : "false");
      Serial.printf("CellOverVoltage %s \n", CellOverVoltage ? "true" : "false");
    */

    if ((receivedBytes_main[139] & 0xF0) == 0x0) {
      Balance_Curr = (((int)receivedBytes_main[139] << 8 | receivedBytes_main[138]) * 0.001);
    } else if ((receivedBytes_main[139] & 0xF0) == 0xF0) {
      Balance_Curr = ((((int)receivedBytes_main[139] & 0x0F) << 8 | receivedBytes_main[138]) * -0.001);
    }

    // Serial.print("Balance_Curr  ");  // umm something to learn ......? "true" : "false"
    // Serial.println(Balance_Curr);

    // +2 byte unbekant 4
    SOC_Percent = ((int)receivedBytes_main[141]);
    //Serial.print("SOC: ");
    //Serial.println(SOC_Percent);

    Capacity_Remain = (((int)receivedBytes_main[145] << 24 | receivedBytes_main[144] << 16 | receivedBytes_main[143] << 8 | receivedBytes_main[142]) * 0.001);
    Nominal_Capacity = (((int)receivedBytes_main[149] << 24 | receivedBytes_main[148] << 16 | receivedBytes_main[147] << 8 | receivedBytes_main[146]) * 0.001);
    Cycle_Count = receivedBytes_main[153] + receivedBytes_main[152] + receivedBytes_main[151] + receivedBytes_main[150];  //((String)receivedBytes_main[153] << 24 | receivedBytes_main[152] << 16 | receivedBytes_main[151] << 8 | receivedBytes_main[150]));
    // +6 byte unbekant 5
    Capacity_Cycle = (((int)receivedBytes_main[161] << 8 | receivedBytes_main[160]) * 0.001);
    Uptime = (((int)receivedBytes_main[164] << 16 | receivedBytes_main[163] << 8 | receivedBytes_main[162]));
    sec = Uptime % 60;
    Uptime /= 60;
    mi = Uptime % 60;
    Uptime /= 60;
    hr = Uptime % 24;
    days = Uptime /= 24;

    // +1 byte unbekannt
    if (receivedBytes_main[166] > 0) {
      charge = "on";
    } else if (receivedBytes_main[166] == 0) {
      charge = "off";
    }
    if (receivedBytes_main[167] > 0) {
      discharge = "on";
    } else if (receivedBytes_main[167] == 0) {
      discharge = "off";
    }
  }



  new_data = false;
}


//==================================================================================================
void Get_Max_Min_Cell_Volts(void) {

  Cell_MaxV = cellVoltage[0];
  Cell_MinV = cellVoltage[0];

  for (int i = 0; i < cell_count; i++) {
    Cell_MaxV = max(cellVoltage[i], Cell_MaxV);
    Cell_MinV = min(cellVoltage[i], Cell_MinV);
  }
  // Serial.print("The maximum value is: "); Serial.println(Cell_MaxV);
  // Serial.print("The minimum value is: "); Serial.println(Cell_MinV);
}

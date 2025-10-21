
/*Pylontech CAN Message ID & contents:-
0x351: Charge and Discharge parameters = (max charge voltage, charge current, discharge current, discharge voltage (latter is not used by Solis))
0x355: SOC & SOH
0x356: Current measurement of Battery Voltage, Current & Temp
0x359: Protection & Alarm flags
0x35C: Battery charge request flags (which I think are ignored by Solis; I don't use them)
0x35E: Manufacturer name ("PYLON ") = ASCII "PYLON" followed by 3 spaces.
*/

bool setup_can_driver() {
  twai_general_config_t general_config = {
    .mode = TWAI_MODE_NORMAL,
    .tx_io = (gpio_num_t)GPIO_NUM_11,
    .rx_io = (gpio_num_t)GPIO_NUM_12,
    .clkout_io = (gpio_num_t)TWAI_IO_UNUSED,
    .bus_off_io = (gpio_num_t)TWAI_IO_UNUSED,
    .tx_queue_len = 100,
    .rx_queue_len = 65,
    .alerts_enabled = TWAI_ALERT_NONE,
    .clkout_divider = 0
  };
  twai_timing_config_t timing_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  esp_err_t error;

  error = twai_driver_install(&general_config, &timing_config, &filter_config);
  if (error == ESP_OK) {
    Serial.println("CAN Driver installation success...");
  } else {
    Serial.println("CAN Driver installation fail...");
    return false;
  }

  // start CAN driver
  error = twai_start();
  if (error == ESP_OK) {
    Serial.println("CAN Driver start success...");
    return true;
  } else {
    Serial.println("CAN Driver start FAILED...");
    return false;
  }
}


void CNA_send_Network_alive_msg() {
  Alive_packet_counter++;
  //Configure message to transmit
  twai_message_t message;
  message.identifier = 0x305;  //DEZ = 773
  message.flags = TWAI_MSG_FLAG_NONE;
  message.data_length_code = 8;
  message.data[0] = Alive_packet_counter & 0xFF;
  message.data[1] = (Alive_packet_counter >> 8) & 0xFF;
  message.data[2] = (Alive_packet_counter >> 16) & 0xFF;
  message.data[3] = (Alive_packet_counter >> 24) & 0xFF;
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;

  //Queue message for transmission
  if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message queued for transmission\n");
  } else {
    printf("Failed to queue CNA_send_Network_alive_msg for transmission\n");
  }
}

void Battery_Manufacturer() {

  twai_message_t message;
  message.identifier = 0x35E;  //DEZ = 862
  message.flags = TWAI_MSG_FLAG_NONE;
  message.data_length_code = 8;
  message.data[0] = 0x50;  //P
  message.data[1] = 0x59;  //Y
  message.data[2] = 0x4c;  //L
  message.data[3] = 0x4f;  //O
  message.data[4] = 0x4e;  //N
  message.data[5] = 0x54;  //T
  message.data[6] = 0x45;  //E
  message.data[7] = 0x43;  //C

  //Queue message for transmission
  if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message queued for transmission\n");
  } else {
    printf("Failed to queue Battery_Manufacturer for transmission\n");
  }
}

void Battery_Request() {
  byte request = 0;
  if (charge == "on") {
    request = request | 0x80;
  } else {
    request = request | 0x00;
  }
  if (discharge == "on") {
    request = request | 0x40;
  } else {
    request = request | 0x00;
  }

  twai_message_t message;
  message.identifier = 0x35C;  //DEZ = 860
  message.flags = TWAI_MSG_FLAG_NONE;
  message.data_length_code = 2;
  message.data[0] = request;
  message.data[1] = 0x00;


  //Queue message for transmission
  if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message queued for transmission\n");
  } else {
    printf("Failed to queue Battery_Request for transmission\n");
  }
}

void Battery_actual_values_UIt() {

  //Configure message to transmit
  twai_message_t message2;
  message2.identifier = 0x356;  //DEZ = 854
  message2.flags = TWAI_MSG_FLAG_NONE;
  message2.data_length_code = 6;
  message2.data[0] = (CAN_Battery_Voltage & 0xFF);
  message2.data[1] = ((CAN_Battery_Voltage >> 8) & 0xFF);
  message2.data[2] = (CAN_Battery_Current & 0xFF);
  message2.data[3] = ((CAN_Battery_Current >> 8) & 0xFF);
  message2.data[4] = (CAN_Battery_T1 & 0xFF);
  message2.data[5] = ((CAN_Battery_T1 >> 8) & 0xFF);

  //Serial.printf("Low Byte %02x\n",(CAN_Battery_Current & 0xFF));     //was just for testing !
  //Serial.printf("High Byte %02x\n",(byte(CAN_Battery_Current >> 8) & 0xFF));
  /*  Saw this in another example...Are we sending signed current????
 *  Current. 0.1 scale, over 2 bytes SIGNED. Not easy. First rescale, then bitshift 
 *  steve- amps is a float
  int16_t int16amps = amps*10;
  CANData356[4] = int16amps;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[5] = int16amps>>8;
*/


  //Queue message for transmission
  if (twai_transmit(&message2, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message2 queued for transmission\n");
  } else {
    printf("Failed to queue Battery_actual_values_UIt for transmission\n");
  }
}

void Battery_limits() {

  //FOR TEST
  // CAN_Adjusted_Battery_charge_current_limit=100;  //10 A
  //Configure message to transmit
  twai_message_t message2;
  message2.identifier = 0x351;  //DEZ = 849
  message2.flags = TWAI_MSG_FLAG_NONE;
  message2.data_length_code = 6;
  message2.data[0] = (CAN_Battery_charge_voltage & 0xFF);
  message2.data[1] = ((CAN_Battery_charge_voltage >> 8) & 0xFF);
  //message2.data[2] = (CAN_Battery_charge_current_limit & 0xFF);
  //message2.data[3] = ((CAN_Battery_charge_current_limit >> 8) & 0xFF);
  message2.data[2] = (CAN_Adjusted_Battery_charge_current_limit & 0xFF);
  message2.data[3] = ((CAN_Adjusted_Battery_charge_current_limit >> 8) & 0xFF);
  message2.data[4] = (CAN_Adjusted_Battery_discharge_current_limit & 0xFF);
  message2.data[5] = ((CAN_Adjusted_Battery_discharge_current_limit >> 8) & 0xFF);
  //message2.data[6] = (CAN_Battery_discharge_voltage & 0xFF);           //UN COMMENT IF YOU WANT TO USE SMA PROTOCOL ON SOLIS USER DEFINED or Pytes / Voltacon Battery types
  //message2.data[7] = ((CAN_Battery_discharge_voltage >> 8) & 0xFF);

  //Queue message for transmission
  if (twai_transmit(&message2, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message2 queued for transmission\n");
  } else {
    printf("Failed to queue Battery_limits for transmission\n");
  }
}

void Battery_SoC_SoH() {


  //Configure message to transmit
  twai_message_t message2;
  message2.identifier = 0x355;  //DEZ = 853
  message2.flags = TWAI_MSG_FLAG_NONE;
  message2.data_length_code = 4;
  message2.data[0] = CAN_SOC_Percent;
  message2.data[1] = 0x00;
  message2.data[2] = CAN_SoH;
  message2.data[3] = 0x00;

  //Queue message for transmission
  if (twai_transmit(&message2, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message2 queued for transmission\n");
  } else {
    printf("Failed to queue Battery_SoC_SoH for transmission\n");
  }
}

void Battery_Error_Warnings() {

  byte ProtectionByte = 0b00000000;  //See Pylon Protocol for Alarms CAN ID: 0x359, Bytes 0 and 2 have flags for error conditions


  if (LowTemp) bitSet(ProtectionByte, 4);
  if (HighTemp) bitSet(ProtectionByte, 3);
  if (CellOverVoltage) bitSet(ProtectionByte, 1);
  if (CellUnderVoltage) bitSet(ProtectionByte, 2);

  //Configure message to transmit
  //A Low or High Temp error should change both message bytes as below
  twai_message_t message2;
  message2.identifier = 0x359;  //DEZ = 857
  message2.flags = TWAI_MSG_FLAG_NONE;
  message2.data_length_code = 7;
  message2.data[0] = ProtectionByte;  //PROTECTION Flags OK= 0x00, 0x10 = BMS Low temp , 0x08 BMS High Temp.  Solis BMS info screen doent show this..Alarm LED ON and msg on main screen.
  message2.data[1] = 0x00;
  message2.data[2] = ProtectionByte;  //ALARM Flags OK =0x00, 0x10 = under temp Solis has no alarm, yet BMS info screen shows BMS Status High temp , 0x08 BMS Low temp
  message2.data[3] = 0x00;
  message2.data[4] = 0x01;
  message2.data[5] = 0x50;
  message2.data[6] = 0x4E;

  //Queue message for transmission
  if (twai_transmit(&message2, pdMS_TO_TICKS(1000)) == ESP_OK) {
    //printf("Message2 queued for transmission\n");
  } else {
    printf("Failed to queue Battery_Error_Warnings for transmission\n");
  }
}


//***********************Charge Adjustment **********************************
//As Cells approach higher terminal voltages and get nearer ful SOC we reduce the max charge current ...(however its difficult to
// accurately relate terminal volts to stored capacity due to absorption time where SOC increases with little to No increase in Cell V

void CAN_Charge_Current_Adjust(void) {

  //Serial.println("Can charge adj.");
  /*Decisions made from the most stringent limit downwards
  
  >3.55V  triggers drop to 0A 
        Current = 0    Stop charging, Force SOC to Solis to be 100% 
  3.45V  Hysterysis --stays at 0A until <3.45V

 >3.46V  triggers 3A 
        Current = 3A   Aid Ballancing
  3.425V  Hysterysis --stays at 3A limit until V Cell max drops <3.45V

 >3.43  triggers 18A 
        Current = 18A   Aid Ballancing
  3.39V Hysterysis --stays at 18A until max V Cell <3.45V when it reverts to the set max limit

  Do these also need to consider SOC or Delta V??..but JK SOC accuracy seems pretty poor. 
  WARNING !! Without some hysterysis, rapid changes in SOC or Charge Current limit values sent over CAN (as in V4) caused spurious OV-ILLC errors in SOLIS 
  */

  //0A limiting and clearing:-
  if (Cell_MaxV > 3550) {  //int divide by 1000 for Volts  THIS WAS IN REV 2 but could stop with cell at 3.5V and only 80%
    CAN_Adjusted_Battery_charge_current_limit = 0;
    CAN_SOC_Percent = 100;                                                             //Tell Inverter battery is full
  } else if ((CAN_Adjusted_Battery_charge_current_limit = 0) && (Cell_MaxV < 3450)) {  //int divide by 1000 for Volts
    CAN_Adjusted_Battery_charge_current_limit = 30;                                    //3A
    CAN_SOC_Percent = SOC_Percent;                                                     //we follow SOC value from BMS
  }

  //3A limiting
  else if (Cell_MaxV > 3460) {                       //&& SOC >85???  3.46V is roughly over 90% charge HOWEVER, LIPO4 SOC can NOT be acurrately related to terminal voltage
    CAN_Adjusted_Battery_charge_current_limit = 30;  //limit to 3A
  } else if ((CAN_Adjusted_Battery_charge_current_limit == 30) && (Cell_MaxV < 3420)) {
    CAN_Adjusted_Battery_charge_current_limit = 180;  //increase limit to 18A
  }

  //18A Limiting
  else if (Cell_MaxV > 3430) {                        // 3.42V is approaching a High SOC  roughly over 90% charge
    CAN_Adjusted_Battery_charge_current_limit = 180;  //limit to 3A
  } else if ((CAN_Adjusted_Battery_charge_current_limit == 180) && (Cell_MaxV < 3390)) {
    CAN_Adjusted_Battery_charge_current_limit = CAN_Battery_charge_current_limit;  //increase limit to max

    //Revert to the Max charge current limit
  } else {
    CAN_Adjusted_Battery_charge_current_limit = CAN_Battery_charge_current_limit;
  }

  /*else if ((SOC_Percent >= 94) || (Delta_Cell_Voltage > 0.0500f)) {  //3.1V is approx 10% SOC,  voltage will drop under a high load, so a lower voltage is used
    CAN_Adjusted_Battery_charge_current_limit = 180;                 //limit to 2A
  }
  else if ((CAN_Adjusted_Battery_charge_current_limit == 180) && (SOC_Percent < 90) && (Delta_Cell_Voltage < 0.030f)) {
    CAN_Adjusted_Battery_charge_current_limit = CAN_Battery_charge_current_limit;  //use max specified charge rate
*/

  Serial.print(" Ave Cell V:- ");
  Serial.println(Average_Cell_Voltage);

  Serial.print(" Max Cell V:- ");
  Serial.println(Cell_MaxV);

  Serial.print(" SOC:- ");
  Serial.println(SOC_Percent);
  Serial.print(" Adjusted CAN I :- ");
  Serial.println(CAN_Adjusted_Battery_charge_current_limit);
}



//**********************************************************************************************

void CAN_Low_Charge_SOC_Control(void) {
  //Sometimes the JK BMS can loose accurate tracking of Battery BMS.
  //The Solis Inverter has over discharge set to 20% - at and below this SOC the Inverter stops discharging the battery
  //At 0% SOC the Solis will forces a charge cycle - Providing Charge from Grid is Enabled but regardless of charging 'Time of use' settings

  //We look for signs of a too Low SOC condition which remains until the battery or cell regains sufficient charge:-
  if (newdatalasttime != 0) {  //we need some passed data values following prog start

    if ((Average_Cell_Voltage < 3.00) || (Cell_MinV < 2900)) {
      Force_Charge = true;  // 0% forces the Solis to Over Discharged state forcing a charge cycle - From PV or AC (providing Charge from Grid is enabled)

    } else if ((Cell_MinV < 2950) || (SOC_Percent < 20)) {
      Halt_Discharge = true;
    }

    if ((Average_Cell_Voltage > 3.12) && (Cell_MinV > 3000)) {  // Battery must charge to clear these Low SOC flags. since cell voltage drops under inverter load, so a lower voltage is used
      Force_Charge = false;
      Halt_Discharge = false;
    }

    if (Force_Charge == true) {
      CAN_SOC_Percent = 0;
      CAN_Adjusted_Battery_discharge_current_limit = 0;
    } else if (Halt_Discharge) {
      CAN_SOC_Percent = 20;  //20% gets sent via CAN overriding the JKs SOC value
      CAN_Adjusted_Battery_discharge_current_limit = 0;

    } else {
      CAN_SOC_Percent = SOC_Percent;  //we follow SOC value from BMS
      CAN_Adjusted_Battery_discharge_current_limit = CAN_Battery_discharge_current_limit;
    }
  }
}
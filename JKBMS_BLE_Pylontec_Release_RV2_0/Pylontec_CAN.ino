
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
  message2.data[4] = (CAN_Battery_discharge_current_limit & 0xFF);
  message2.data[5] = ((CAN_Battery_discharge_current_limit >> 8) & 0xFF);
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
  message2.data[0] = SOC_Percent;
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

byte ProtectionByte = 0b00000000;        //See Pylon Protocol for Alarms CAN ID: 0x359, Bytes 0 and 2 have flags for error conditions


if(LowTemp)bitSet(ProtectionByte,4);
if(HighTemp)bitSet(ProtectionByte,3);
if(CellOverVoltage)bitSet(ProtectionByte,1);
if(CellUnderVoltage)bitSet(ProtectionByte,2);

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
//As SOC approaches 100 % or if Cell Delta V is large we reduce the current to add balancing

void CAN_Charge_Current_Adjust() {
  Serial.println("Can charge adj.");
  //Serial.println(Delta_Cell_Voltage);  Cell_MaxV

  //if (Average_Cell_Voltage > 3.40 || Delta_Cell_Voltage > 0.0600f) {
  //  CAN_Adjusted_Battery_charge_current_limit = 18;  //Slow Charging
  // Serial.printf("Ave CellV >3.5 or delt >60mV, - Charging limited to 1.8A \n");

    if ((SOC_Percent >=98 )|| (Cell_MaxV > 3500)) {                    //int divide by 1000 for Volts
    CAN_Adjusted_Battery_charge_current_limit = 20;                    //limit to 2Amps?
      Serial.printf("Cells >3.5 or SOC>=98, Halt charging,  Current limit 2A \n");
    }

     if ((SOC_Percent <=97) && (Cell_MaxV < 3400)) {                    //int divide by 1000 for Volts
    CAN_Adjusted_Battery_charge_current_limit = CAN_Battery_charge_current_limit;
    Serial.printf("SOC <97% & Cell maxV <3.45 all ok...Default 50A set limit applies \n");
    }
}

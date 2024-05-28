/* 
based on code from https://www.akkudoktor.net/forum/open-source-software-projekte/jkbms-auslesen-ueber-ble-bluetooth-oder-rs485-adapter-mittels-eps-iobroker/paged/49/
Big thanks to Scotty89 and others for their incredible brains and sharing 
CAN Code updated for ESP32 S3 versions. (CAN routines deprocated in S3,  renamamed TWAI. Should now work on ESP32, and S3 variants
DO NOT INSTALL ESP32 BLE Library by Neil Kolban! - if its in your Documents/arduino/Libraries folder , remove it!
The ESP32 BLE Lib is now installed and maintained as part of the ESP32 by Espressif boards manager package
Added MTU 517 in connection.ino to stop S3 crashing! , CAN works but displays need sorting! Can delays be reduced ?
added charging reduction at Hi dV or high SOC. Main Display sorted, Needs cell display and CAN Display?
Fixed charge algorithm, Cell volts array changed to int, Added Cell 1-8 display
Added Cell screen for upto 16 cells & CAN screen
Changed scan detect to BLE address works quicker and device name is completely un reliable in this and nimble libs
Added touch display changing, timeout, Charge limiting gave inverter errors?? reviewed lifepoe charge volts
Adding Reading of Alarm flag bits - wrong byte positions!
Over & Under temp warnings added. Configured and Working on PCB.  
Display now sleeps, added Cells 9-16 display, Added Display activetimeout (Pixel burn in saver, new cell screen, Graphics on BLE scan and boot screen)
TO DO .... Support & Test with JKBMS V11 Hardware!?
*/
//DONT MAKE COMMENT BLOCKS TOO BIG - Arduino IDE can throw odd compilation errors!!
/*
Release R1.0
Last updated 28-05-2024
Written by Steve Tearle
Before using:-
Specify your number of Cells ...usually 8 15 or 16
Set the BLE address of your JKBMS 
Specify your required Charge V and I specs and limits
Set if you want CAN active true false
*/

#include "BLEDevice.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "Arduino.h"
#include "TFT_eSPI.h"  // This library will need installing and setting up to suit the t-display S3,  see https://youtu.be/gpyeMjM9cOU?si=xTou63YCcEhUK2XR
#include "Boot_Splash_Screen.h"
#include "BLE_Signal_Grey.h"  // .h files containing various bitmap images 
#include "BLE_Signal_Blue.h"
#include "BLE_Signal_Green.h"
#include "Warningtriangle3.h"

TFT_eSPI lcd = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&lcd);

#define SW_Version "R1.0"

bool debug_flg = true;
bool debug_flg_full_log = false;

//########### Hardware Settings / Options  #########

// CAN Settings
bool CAN_use = true;  // true if CAN module is required for the Paylontech emulation
const int CAN_Update_Interval = 2000;
bool CAN_OK = false;  //we use this to identify if CAN/TWAI is setup and Running ok

//BMS-BLE Settings
const char* Geraetename = "JK-B2A24S15P";    //JK-B2A24S20P JK-B2A24S15P
const char* BLE_ADDR = "C8:47:8C:E5:20:84";  //This is the BLE MAC address of your specific JK BMS - find in serial monitor or use BLE app on phone

//Set your BMS Hardware Version - Data parsing is different wrong or erratic values given if not correct
#define BMS_Version 10  //  is your BMS Hardware /Firmware Ver 10 or 11? - see B.T App 'About'

// Set your number of cells
#define cell_count 16

//BMS Battery Settings - adjust values to suit battery and desired charging limits
//CAN_device_t CAN_cfg;

int16_t CAN_Battery_Voltage = 0;                    //  (0.01,0) [0|327.67] "V" Vector__XXX
int16_t CAN_Battery_T1 = 0;                         //(0.1,0) [-50|75] "°C" Vector__XXX
int16_t CAN_Battery_Current = 0;                    // [-250|250] "A" Vector__XXX
uint16_t CAN_Battery_charge_voltage = 558;          // 0,0 - 500,0 V  ,   i.e   553 = 55.3V = 3.46V/Cell in 16 cell pack
int16_t CAN_Battery_charge_current_limit = 500;     // in mA/100  0,0 - 500,0 A   ie 550 = 55000mA = 55A
int16_t CAN_Battery_discharge_current_limit = 550;  // in mA/100 -500,0 - 0,0  A
uint16_t CAN_Battery_discharge_voltage = 496;       // 0,0 - 6553,5 V   49.6v = 3.1V/cell x 16
byte CAN_SoH = 100;                                 // Batter State of Health

/*FYI LiFePO4 charging for optimum cycle life is discussed here https://diysolarforum.com/threads/best-lifepo4-charge-controller-settings-known-to-man-for-maximum-service-life-and-minimum-battery-stress-5-000-10-000-cycles.34813/
  BULK/ADSORB 3.45-3.52V/cell [heavily debated] but universally agreed no higher than 3.52 for max service life [3.52 is what victron recommends for max service life, I chose 3.46Vx16
  FLOAT 3.35/cell   3.35 volts is a good float voltage for high reserve capacity and minimal cell
  degradation [3.35 is not really debated if maximum service life is the goal 3.4 if you want a little bit more reserve. Also 3.4 is recommended by many OEMS
  CUT OFF 3.1V/cell [but a more ideal verifying the cut off voltage is is to run down battery pack under average load you will be running and then take note of what cell voltage drops off first, once the first cell drops off in voltage
  measure the total pack voltage and set low cut off voltage to that measured total pack voltage.
*/

//########### End of "User" Settings #######################################################################

//Display Settings
//#define LCD_Backlight 38                //This line controls the T-Display S3's Backlight
unsigned long LCD_On_Duration = 30000;  // To stop burn-in of display and to save power we put display to sleep after this time, if zero Display on 24/7
bool DisplayActive = true;              //At Power up the display is Active, uses to identify if the display is currently active or asleep
unsigned long DisplayActivated_mS = 0;  //used to manage the Sleeping timeout of the display

unsigned long LastDisplayUpdate_mS = 0;  //for managing display interval timing
const int DisplayInterval = 500;         //Display values updated every 500mS

int DisplayID = 0;                       //Which Screen do we show? 0=main screen, 1= Cells1-8, 2=Cells9-16, 3= All Cells, 4=PylonLV, 5 Warnings,
unsigned long LastDisplayChange_mS = 0;  //touchpad cycles through screens...after timeout we return to main screen



static BLEUUID serviceUUID("ffe0");  // The remote service we wish to connect to.
static BLEUUID charUUID("ffe1");     //ffe1 // The characteristic of the remote service we are interested in.
BLEClient* pClient;
BLEScan* pBLEScan;
byte getdeviceInfo[20] = { 0xaa, 0x55, 0x90, 0xeb, 0x97, 0x00, 0xdf, 0x52, 0x88, 0x67, 0x9d, 0x0a, 0x09, 0x6b, 0x9a, 0xf6, 0x70, 0x9a, 0x17, 0xfd };  // Device Infos
byte getInfo[20] = { 0xaa, 0x55, 0x90, 0xeb, 0x96, 0x00, 0x79, 0x62, 0x96, 0xed, 0xe3, 0xd0, 0x82, 0xa1, 0x9b, 0x5b, 0x3c, 0x9c, 0x4b, 0x5d };

unsigned long sendingtime = 0;
unsigned long bleScantime = 0;
unsigned long mqttpublishtime = 0;
unsigned long canpublishtime = 0;


unsigned long newdatalasttime = 0;
unsigned long ble_connection_time = 0;

unsigned long LastLoopTime = 0;
int LoopCount = 0;
bool Interval_First_Loop_Pass = false;

byte receivedBytes_main[320];
int frame = 0;
int frame_mqtt = 0;
bool received_start = false;
bool received_start_frame = false;
bool received_complete = false;
bool new_data = false;
byte BLE_Scan_counter = 0;

//BMS Values
int cellVoltage[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //make this bigger if you have more than 16 cells
int Cell_MaxV = 0;                                                         //this is int,  /1000.0 to get Volts
int Cell_MinV = 0;

//float cellVoltage[32];
float balanceWireResist[32];
float Average_Cell_Voltage = 0;
float Delta_Cell_Voltage = 0;
int Current_Balancer;
float Battery_Voltage = 0;
float Battery_Power = 0;
float Charge_Current = 0;
float Battery_T1 = 0;
float Battery_T2 = 0;

float MOS_Temp = 0;
int SOC_Percent = 0;
float Capacity_Remain = 0;
float Nominal_Capacity = 0;
String Cycle_Count = "";
float Capacity_Cycle = 0;
uint32_t Uptime;
uint8_t sec, mi, hr, days;
float Balance_Curr = 0;
String charge = "off";
String discharge = "off";
bool LowTemp = false;  //Various Alarm / Error  flags ....
bool HighTemp = false;
bool CellOverVoltage = false;
bool CellUnderVoltage = false;

// Adjusted CAN data
int CAN_Adjusted_Battery_charge_current_limit;  // modified charge current value, use to create an enhanced & personalised charging and battery care strategy...i.e reduce charge rate at high delta V or towards temperature extremes   charging current as part of a in mA/100  0,0 - 500,0 A   ie 550 = 55000mA = 55A

static bool doConnect = false;
static bool ble_connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

unsigned long Alive_packet_counter = 0;

#define grey 0x6B6D
#define blue 0x0967
#define orange 0xC260
#define purple 0x604D
#define green 0x1AE9


//TouchPad
int threshold = 1500;  // ESP32S2
bool touch10detected = false;

void gotTouch10() {  //Callback for touch interupt
  touch10detected = true;
}

//--------------------------------------------------------


static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  char data[320];
  String topic;
  if (debug_flg_full_log) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");

    for (int i = 0; i < length; i++) {
      Serial.print(pData[i], HEX);
      Serial.print(", ");
    }
    Serial.println("");
  }
  if (pData[0] == 0x55 && pData[1] == 0xAA && pData[2] == 0xEB && pData[3] == 0x90 && pData[4] == 0x02) {
    Serial.printf("Found Frame Start...");
    received_start = true;
    received_start_frame = true;
    received_complete = false;
    frame = 0;
    for (int i = 0; i < length; i++) {
      receivedBytes_main[frame] = pData[i];
      frame++;
    }
  }

  if (received_start && !received_start_frame && !received_complete) {
    //     Serial.println("Daten erweitert !");


    for (int i = 0; i < length; i++) {
      receivedBytes_main[frame] = pData[i];
      frame++;
    }
    if (frame == 300) {
      Serial.println("...and Frame Complete");
      received_complete = true;
      received_start = false;
      new_data = true;
      BLE_Scan_counter = 0;
      char data[20];

      if (debug_flg_full_log) {
        for (int i = 0; i <= frame; i++) {
          sprintf(data, "Frame:%d 0x%02X ", i, receivedBytes_main[i]);
          Serial.print(data);
        }
      }
    }
    if ((frame > 300)) {

      Serial.println("Fehlerhafte Daten !!");
      frame = 0;
      received_start = false;
      new_data = false;
    }
  }
  //Serial.print("frame: ");
  //Serial.println(frame);
  received_start_frame = false;
}

class MyClientCallback : public BLEClientCallbacks {

  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    ble_connected = false;
    //pclient->disconnect();
    Serial.println("BLE-Disconnect");
    delay(200);
    Serial.println("BLE was Disconnected ... and no BLE reconnection possible, Reboot ESP...");
    ESP.restart();
  }
};

//=================================================================================================
/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
       Called for each advertising BLE server.
    */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    lcd.pushImage(220, 45, 75, 75, BLE_Signal_Blue);
    delay(100);
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());


    //Show name of advertising device on display
    lcd.setTextColor(TFT_CYAN, TFT_BLACK);
    //sprite.drawString("                         ", 8, 145, 2);

    lcd.setFreeFont(&FreeSerif9pt7b);
    lcd.setTextPadding(245);  // Blanked area will be 245 pixels wide

    if (advertisedDevice.getName() != "") {
      lcd.drawString(advertisedDevice.getName().c_str(), 8, 145);
      //sprite.drawString("Un-Named Device",8,145,4);
    } else {
      //sprite.drawString(advertisedDevice.getName().c_str(),8,145,2);
      lcd.drawString(advertisedDevice.getAddress().toString().c_str(), 8, 145);  //
    }
    lcd.setTextPadding(40);  // Blanked area will be 245 pixels wide

    lcd.setTextColor(TFT_CYAN, TFT_BLACK);
    lcd.drawString(String(advertisedDevice.getRSSI()), 254, 145);  //was font 2
    lcd.drawString("dB", 289, 145);

    lcd.pushImage(220, 45, 75, 75, BLE_Signal_Grey);
    delay(100);



    // We have found our BMS address & device
    // Lets see if it contains the service we are looking for. BLE_ADDR Serial.println(advertisedDevice.getAddress().toString());
    // if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID) && advertisedDevice.getName() == Geraetename) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID) && (advertisedDevice.getAddress() == BLEAddress(BLE_ADDR))) {

      lcd.pushImage(220, 45, 75, 75, BLE_Signal_Green);
      lcd.drawRoundRect(5, 40, 180, 80, 4, TFT_BLUE);
      lcd.setFreeFont(&Orbitron_Light_24);
      lcd.setTextColor(TFT_GREEN, TFT_BLACK);
      lcd.drawString("Found our ", 30, 60);
      lcd.drawString("    BMS      ", 30, 90);

      delay(500);
      //DisplayBMSFound();

      pBLEScan->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      pBLEScan->clearResults();

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks

//************* SETUP FOLLOWS *************************************************
void setup() {

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);  // This is required when powering the T-Display S3 from is +5V pin (..it enables a supply to the LCD  which usually comes from VBUSS on USB)


  Serial.begin(115200);
  Serial.print("BMS Interface V ");
  Serial.println(SW_Version);
  Serial.println("Booting");

  //Pylontech CAN/ TWAI
  if (CAN_use) {
    CAN_OK = setup_can_driver();  //start CAN Module, Returns true if OK
  }

  //BLE Setup
  BLEDevice::init("");
  pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(260);  //was 1349 ?!
  pBLEScan->setWindow(220);    //was 449  ?!
  pBLEScan->setActiveScan(true);

  //Initialise LCD

  lcd.init();
  //lcd.fillScreen(TFT_BLACK);
  lcd.setRotation(1);   //Display is rotated right to landscape
  lcd.setTextDatum(3);  //Text locate according to middle left ...needed to get certain fonts near top pixel row
  sprite.createSprite(320, 170);
  sprite.setTextDatum(3);
  sprite.setSwapBytes(true);

  //CAN_Display();       //these are here just to test out new screen layouts at power up
  //UpdateDisplay2();
  //ExtraInfoScreen();

  delay(100);          // it seems some small delay is needed following LCD init
  BootSplashScreen();  //JK BMS and PV image & details
  delay(2500);

  //For Touch button
  touchAttachInterrupt(T10, gotTouch10, threshold);

  //LEDC PWM is used for LCD backlight brightness adjustment, display fades out after timeout
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);  //pin 38 controlls Backlight on T-Display S3, High is 255 = full On,  This pin is on Channel 0
  ledcWrite(0, 254);     // For some reason dimming down from 255 to 254..253 etc causes a jump in brightness..DC to a 10kHz change?!?

  ScanScreen();  //Optional place to add a welcome screen
  delay(300);


}  // End of setup.


//***************** MAIN LOOP **************************************************
void loop() {

  //touch Button
  if (touch10detected) {
    touch10detected = false;
    if (touchInterruptGetLastStatus(T10)) {
      DisplayID++;
      LastDisplayChange_mS = millis();
      DisplayActivated_mS = millis();

      if (DisplayID == 6) {
        DisplayID = 0;
      }
      if (!DisplayActive) {
        DisplayActive = true;
        DisplayID = 0;       //activate display and start on main screen
        Activate_display();  //Power up backlight and initialise the display
      }

      //DisplayActive = true;  // If the display is not active and initialised
      //following a touch detection, the Display will start a new powered-up activation duration
      Serial.printf("T10 Touched  %i \n", DisplayID);
    } else {
      //Serial.println(" --- T10 Released");
    }
  }


  //Manage BLE
  if (doConnect == true) {
    if (!connectToBLEServer()) {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");

      delay(500);
      ble_connected = false;
      doConnect = false;
    }
  }


  if (ble_connected) {

    if (received_complete) {
      //      for (int i = 0; i < 319; i++)  {
      //        Serial.print(receivedBytes_main[i],HEX);
      //       Serial.print(", ");
      //      }
      //      Serial.println("");
      if (new_data) {
        ParseData();
        Get_Max_Min_Cell_Volts();
        newdatalasttime = millis();
      }
    }

    //CAN Updating
    if ((canpublishtime == 0) || (millis() >= (canpublishtime + CAN_Update_Interval))) {
      canpublishtime = millis();
      Serial.println("CAN Tx");

      if (CAN_use) {
        CAN_Charge_Current_Adjust();
        //CNA_send_Network_alive_msg();
        Battery_actual_values_UIt();
        Battery_Manufacturer();
        Battery_Request();
        Battery_limits();
        Battery_SoC_SoH();
        Battery_Error_Warnings();
      }
    }

    else {  //Update Display Contents...if updating CAN was true, then we wont update display on the same loop pass

      if ((LastDisplayUpdate_mS == 0) || ((millis() - LastDisplayUpdate_mS) >= DisplayInterval)) {  // we service the display every 0.5 Sec
        //200mS after CAN updating we updade displays
        //Update relevant Display
        LastDisplayUpdate_mS = millis();
        if (ble_connected) {
          UpdateDisplay();
        }
      }
    }



    // BLE Get Device Data Trigger ...
    if (((millis() - sendingtime) > 500) && sendingtime != 0) {  // millis() >  sendingtime + sendingtimer aktive_sending &&
      sendingtime = 0;
      Serial.println("Sending Info Request");
      //aktive_sending = false;
      pRemoteCharacteristic->writeValue(getInfo, 20);
    }
  }

  //BLE nicht verbunden
  if ((!ble_connected && !doConnect && (millis() - bleScantime) > 4000)) {  //time was 15000, trying 4000

    Serial.println("BLE  ESP.restart();-> Reconnecting " + String(BLE_Scan_counter));
    bleScantime = millis();
    pBLEScan->start(3, false);  //this is a blocking call ...code carries on after 3 secs of scanning.....
    BLE_Scan_counter++;
  }

  // BLE verbidnugn ist da aber es kommen seit X Sekunden keine neuen Daten !
  if (!doConnect && ble_connected && (millis() >= (newdatalasttime + 60000)) && newdatalasttime != 0) {
    ble_connected = false;
    delay(200);
    newdatalasttime = millis();
    pClient->disconnect();
  }

  //checker das nach max 5 Minuten und keiner BLE Verbidung neu gestartet wird...
  if (BLE_Scan_counter > 20) {
    delay(200);
    Serial.println("BLE isn´t receiving new Data form BMS... and no BLE reconnection possible, Reboot ESP...");
  }

}  // End of loop

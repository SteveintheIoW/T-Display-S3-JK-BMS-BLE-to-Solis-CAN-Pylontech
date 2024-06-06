<div align = center>

# [Arduino Lilygo T-Display S3 JK-BMS Wireless BLE To Pylontech CAN interface]()

</div>
A Lilygo T-Display S3 connects to a JK-BMS via BLE and then transfers the relevant data values to CAN bus using Pylontech LV protocol. JK BMS's (eg JK-B2A24S15P etc) are widely used and highly respected BMS designs having a 2A balance capability, however they lack any built in CAN bus protocol compatible to any modern Hybrid energy storage inverters. This project provides a convenient and reliable solution for interfacing a JK BMS to a Hybrid Inverter such as the Solis EH1 & RHI etc.<br/>
The T-Display S3's impressive colour screen is used to present a number of pages displaying key BMS information, alarms and CAN bus Data.<br/> 

Please Note:  Currently this has only been tested to work on a JK-B2A24S15P vith Hardware / Software version 10. It is very likely some small tweaks may be required to work with a Version 11 BMS model (any feedback or code fixes are greatly welcomed!).  <br/>

Based on [https://github.com/syssi/esphome-jk-bms and https://github.com/maxx-ukoo/jk-bms2pylontech](https://www.akkudoktor.net/forum/open-source-software-projekte/jkbms-auslesen-ueber-ble-bluetooth-oder-rs485-adapter-mittels-eps-iobroker/paged/49/).<br/>

Huge thanks must go to Scotty89 and others for their far superior coding skills and generosity in sharing their work.<br/>

<br/>

# Features
- Connects wirelessly and reliably to the JK-BMS via BLE and then outputs Pylontech LV CAN frames.
- Basic adjustment of Max charge current based on SOC and Delta Cell voltage is used to improve cell balancing and hopefully extend Cell Cycle life.
- The T-Display provides a number of pages displaying BMS information, SOC, Cell voltages, Temperatures, Alarms etc in colour.
- A simple touch pad located inside the enclosure is used to wake the display and to cycle through different LCD pages.
- The display's backlight is dimmed following a timeout period. The display's driver is then disabled to reduce power and avoid Pixel burn-in.
- A PCB has been created to fit a Multicomp MC001067 IP65 Polycarbonate Enclosure with clear lid and mounting flanges (55x82x80mm).
- Using an enclosure with a clear lid and having the internal touch pad sensing simplifys assembly and result in a tidy design.
- A Serial output can support monitoring and debugging.
- Even if you dont require a CAN interface, perhaps this project offers an attractive and simple BMS display option with no cable or plug connections to the BMS? 
  (simply set 'CAN_use=false')

The basic interface circuit is very simple and can use just 3 key components:- a T-Display S3, a SN65HVD230 CAN bus transceiver PCB module and an RJ45 connector.
The T-Display and circuitry can then be powered from the 5V on it's USB C socket (although in some cases this may give you some issues maintaining USB power in a grid power outage). Alternatively the interface can be powered from the battery bank using a number of different linear or DC-DC step down switching regulator options. 

<br/>

![Overview](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Enclosure%20and%20PCB3.png)

<br/>

# Screenshots
 A number of pages display Battery data, CAN information and BMS Status:-

| Maximum Cell Statistics page with total Time of Balancing | Maximum Cell Statistics page |
| ![Max page](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Main%20Screen.jpg) | ![Min page](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/All%20Cell%20Volts%202.jpg)  |
| 
<br/>

# Pictures

| My installation | 
| :-: | :-: |
| ![My installation](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Interface%20and%20Solis%20EH1%20System2.jpg) | ![Automatic brightness](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/AutomaticBrightness.jpg) |
| Breadboard detail |  |
| 
<br/>

# Connection schematic

- EasyEda [schematics](https://easyeda.com/editor#id=0d1a2556b7634c8bbd22e9c0474cd401) and [PCB layout](https://easyeda.com/editor#id=623a04630b8b4449b72bd5462f59e85f) by Ngoc Dang Dinh.

# Board pinout diagrams
- [Nano](https://store.arduino.cc/products/arduino-nano#docs)
- [Uno](https://store.arduino.cc/products/arduino-uno-rev3#docs)
 <br/>

# Compile with the Arduino IDE
Download and extract the repository. In the Arduino IDE it is necessary to install the Espressif ESP32 board library via board manager. 
It is also necessary to install and configure the TFT_eSPI library.  See various online resources for the T_Display S3 setup such as (https://youtu.be/gpyeMjM9cOU?si=xTou63YCcEhUK2XR)
.<br/>


# Compile options / macros for this software
To customize the software to different requirements, there are some compile options / macros available.<br/>
Modify them by enabling / disabling them, or change the values if applicable.

| Name | Default value | Description |
 <br/>

# BOM
### Required
- Breadboard.
- Jumper wire.
- Pin header to connect cables to breadboard.
- Schottky diode e.g. BAT 42, BAT 43, BAT 85.
- Arduino Nano.
- 16 MHz crystal.
- MCP2515 / TJA1050 kit for Arduino. !!! You must replace the assembled 8 MHz crystal with a 16 MHz one !!!

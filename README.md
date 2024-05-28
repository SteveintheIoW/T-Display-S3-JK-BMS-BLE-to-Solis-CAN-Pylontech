<div align = center>

# [Arduino Lilygo T-Display S3 JK-BMS To Pylontech CAN interface]()

Converts the JK-BMS BLE data to Pylontech LV CAN data thus providing an interface to Hybrid Inverters such as Solis EH1 & RHI.<br/>
The T-Display S3 offers an impressive colour screen to present a number of pages displaying BMS information, alarms and CAN bus Data.<br/>
Please Note:  Currently this has only been tested to work on a JK-B2A24S15P vith Hardware and Software version 10.<br/>


Based on [https://github.com/syssi/esphome-jk-bms and https://github.com/maxx-ukoo/jk-bms2pylontech](https://www.akkudoktor.net/forum/open-source-software-projekte/jkbms-auslesen-ueber-ble-bluetooth-oder-rs485-adapter-mittels-eps-iobroker/paged/49/).<br/>

Big thanks go to Scotty89 and others for their skills and generosity in sharing their work.

</div>


# Key Features
- Connects wirelessly and reliably to the JK-BMS via BLE and then outputs Pylontech LV CAN frames.
- Basic adjustment of Max charge current based on SOC and Delta Cell voltage is used to improve cell balancing and hopefully extend Cell Cycle lifetime.
- T-Display provides a number of pages showing BMS information, SOC, Cell voltages, Temperatures, Alarms etc in colour.
- A simple touch pad locaded inside the enclosure is used to wake the display and also cycle through different LCD pages.
- Display backlight is dimmed following a timeout period, the display is then disabled to reduce power and avoid Pixel burn-in.
- A PCB has been created to fit a Multicomp MC001067 IP65 Polycarbonate Enclosure with clear lid and mounting flanges (55x82x80mm) .
- Serial output supports monitoring and debugging.

The basic interface circuit is very simple and can use just a T-Display S3, a SN65HVD230 CAN bus transceiver PCB module and RJ45 connector.
A 5V supply to the T-Display may be derived from is USB C socket or a number of DC-DC step down regulator options. 

<br/>

![Overview](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/BreadboardAndOverviewPage.jpg)

<br/>

# Screenshots
 The screenshots are taken from the [Wokwi example](https://wokwi.com/projects/371657348012321793) with `STANDALONE_TEST` enabled and therefore may contain random data.


| Maximum Cell Statistics page with total Time of Balancing | Maximum Cell Statistics page |
| ![Max page](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/StatisticsMaxPage.png) | ![Min page](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/StatisticsMinPage.png)  |
| CAN info page |  |
| 
<br/>

# Pictures

| My installation | Automatic brightness |
| :-: | :-: |
| ![My installation](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/CompleteInstallation.jpg) | ![Automatic brightness](https://github.com/ArminJo/JK-BMSToPylontechCAN/blob/main/pictures/AutomaticBrightness.jpg) |
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

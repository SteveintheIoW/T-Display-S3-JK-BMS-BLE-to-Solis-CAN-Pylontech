<div align = center>

# [Arduino Lilygo T-Display S3 JK-BMS Wireless BLE To Pylontech CAN interface]()

</div>
A Lilygo T-Display S3 connects to a JK-BMS via BLE and then transfers the relevant data values to CAN bus using Pylontech LV protocol. JK BMS's (eg JK-B2A24S15P etc) are widely used and are a highly respected BMS designs having a 2A balance capability, however they lack any built in CAN bus protocol compatible to any modern Hybrid energy storage inverters. This project provides a convenient and reliable solution for interfacing a JK BMS to a Hybrid Inverter such as the Solis EH1 & RHI etc.<br/>
The T-Display S3's impressive colour screen is used to present a number of pages displaying key BMS information, alarms and CAN bus Data.<br/> 

**Please Note:**  Currently this has only been tested to work on a JK-B2A24S15P with Hardware / Software version 10. It is very likely some small tweaks may be required to work with a Version 11 BMS model (any feedback or code fixes are greatly welcomed!). I've also only been able to test CAN functionality on a Solis S5 EH1 but the CAN (now TWAI) code is reported to work OK on other inverter types.  <br/>

Based on [https://github.com/syssi/esphome-jk-bms and https://github.com/maxx-ukoo/jk-bms2pylontech](https://www.akkudoktor.net/forum/open-source-software-projekte/jkbms-auslesen-ueber-ble-bluetooth-oder-rs485-adapter-mittels-eps-iobroker/paged/49/).<br/>

Huge thanks must go to Scotty89 and others for their far superior coding skills, hard work, and generosity in sharing their code.<br/>

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
- I sought a simple, reliable device to interface between my trusted JK BMS and a new Solis Hybrid Inverter, I had no requirement or interested in ESPHome or WiFi 
  connectivity etc.  
- Even if you dont require a CAN interface, perhaps this project offers an attractive and simple BMS display option with no cable or plug connections to the BMS? 
  (simply set 'CAN_use=false')
  
The basic interface circuit is very simple and can use just 3 key components:- a T-Display S3, a SN65HVD230 CAN bus transceiver PCB module and an RJ45 connector.
The T-Display and circuitry can then be powered from the 5V on it's USB C socket (although in some cases this may present issues in maintaining USB power during a grid power outage). Alternatively the interface can be powered from the main battery bank using a number of different linear or DC-DC step down switching regulator options. 

<br/>

![Overview](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Enclosure%20and%20PCB3.png)

<br/>

# Screenshots
 The simple circuitry shown on a breadboard.  A number of pages display Battery data, CAN information and BMS Status:-

| **Main Screen -** Current is shown Green for Charging, Red for Discharge|
![Max page](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Main%20Screen.jpg) 

| **Max Min Cell Volts -** All Min values are shown in Blue, Max values shown in Red|
![Min page](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/All%20Cell%20Volts%202.jpg)  

 | **Max Min Cell Volts -** All Min values are shown in Blue, Max values shown in Red|
![Min page](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/All%20Cell%20Volts%202.jpg)  
 
<br/>

# Pictures

| My installation - Solis EH1 | 
| ![My installation](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/Interface%20and%20Solis%20EH1%20System2.jpg) 
|  
<br/>


# Overview Video

- Video of basic operation  [Youtube](https://youtu.be/v3kCdW38NNI) 


# Circuit Schematics & PCB

- Schematic Circuit Diagram pdfs [Simplest Circuit for interfacing](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/T-Display%20S3%20%20JKBMS%20BLE%20to%20CAN%20Interface%20%20-%20relevant%20parts.pdf) and [Full Circuit Diag for PCB](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/T-Display%20S3%20%20JKBMS%20BLE%20to%20CAN%20Interface%20%20-%20Full%20PCB%20cct%20Diag.pdf) 

PCB Layout [Top Side](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/PCB%20Photo%20View_Top.svg) |
[Bottom Side](https://github.com/SteveintheIoW/T-Display-S3-JK-BMS-BLE-to-Solis-CAN-Pylontech/blob/main/Photos_Media/PCB%20Photo%20View_Bottom.svg) 

The PCB is designed to be flexible, it has many superfluous component positions offering potential to support future I/O expansion ideas (i.e RS485 / MODBUS Comms to say the inverter or a power meter, HC-12 433MHZ transciever module - long distance data to remote devices / displays, an Open drain FET - maybe control a battery heater based on BMS temperature data?)

If you just require one or two pcbs then I may be able to supply them far quicker and cheaper than ordering a batch of boards with shipping and tax etc. Drop me a mail to PVhotwater@btinternet.com

# Compile with the Arduino IDE
Download and extract the repository. In the Arduino IDE it is necessary to install the Espressif ESP32 board library via board manager. 
It is also necessary to install and configure the TFT_eSPI library.  See various online resources for the T_Display S3 setup such as (https://youtu.be/gpyeMjM9cOU?si=xTou63YCcEhUK2XR)

You will need to edit the source code to identify the BLE address of your specific BMS, also review the number of cells and charge voltages and currents to suit your battery setup.
.<br/>





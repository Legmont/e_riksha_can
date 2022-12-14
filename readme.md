# ERiksha CAN-Bus Prototype
Hochschule Karlsruhe
Lecture: Energieeffiziente Mikrocontroller, WS2022/23
brse1011@h-ka.de
nejo1017@h-ka.de

### Project description
In this project a CAN-Bus prototype for a ERiksha was developed and optimized regarding the energy efficiency.

### Hardware
- CAN-Master: ESP32 Dev Module
- CAN-Participant: WEMOS LOLIN32 Lite
- Texas Instruments SN65HVD230 CAN Adapter

### CAN-Library
In this Project a CAN-Library was used. As described in the following Link:
http://www.iotsharing.com/2017/09/how-to-use-arduino-esp32-can-interface.html

### Setup
1. first install Arduino 2 IDE
2. Open the project can_master_esp32 and configure the board ESP32 Dev Module and the correct port, then upload to the board
3. Open the project can_participant_esp32 and configure the board WEMOS LOLIN32 Lite and the correct port, then upload to the board

### Usage
touch GPIO 15 on the ESP32 (Master) to wake the ESP32 up from deep-sleep, the ESP will then send a CAN-Message to the other board,
the Lolin board will wake up from the message and, if it has the correct ID, toggle the LED.






/*****************************************************************
* ERiksha CAN-Prototype Participant
* WS2022/23 
* Projektteilnehmer: brse1011, nejo1017
* IDE: Arduino 2.0.3
* Hardware: - WEMOS LOLIN32 Lite
            - Texas Instruments SN65HVD230 CAN Adapter
  Usage: Connect Lolin Board to USB Power
*******************************************************************/
#include "src/ESP32CAN.h"
#include "src/CAN_config.h"
/////////////////////////////
///THIS//IS//ON//THE//LOLIN//
/////////////////////////////
/////////////////////////////
///Check//RX//AND//TX//PINS//
/////////////////////////////
CAN_device_t CAN_cfg;
RTC_DATA_ATTR int bootCount = 0;
void setup() {
    ++bootCount;
    pinMode(25, OUTPUT); // Set GPIO22 as digital output pin
    pinMode(15,INPUT);
    Serial.begin(115200);
    Serial.println("iotsharing.com CAN demo");
    CAN_cfg.speed=CAN_SPEED_1000KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5; //5 ori
    CAN_cfg.rx_pin_id = GPIO_NUM_4; //4
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //initialize CAN Module
    ESP32Can.CANInit();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0);
    if(bootCount<2){
    esp_deep_sleep_start();
    }
}
void loop() {
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){
      if(rx_frame.MsgID == 1){
        if(rx_frame.FIR.B.RTR==CAN_RTR)
          printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        else{
          printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
          for(int i = 0; i < rx_frame.FIR.B.DLC; i++){
            printf("%c\t", (char)rx_frame.data.u8[i]);
          }
          if(rx_frame.data.u8[0] == 't'){
            digitalWrite(25, !digitalRead(25)); //toggle LED
          }
        }
      }else{
        esp_deep_sleep_start();
      }
    }
    if(digitalRead(25)== LOW){
      esp_deep_sleep_start(); //Start Sleep
    }
}

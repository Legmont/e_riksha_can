/*****************************************************************
* ERiksha CAN-Prototyp Master
* WS2022/23 
* Projektteilnehmer: brse1011, nejo1017
* IDE: Arduino 2.0.3
* Hardware: - ESP32 Dev Module
            - Texas Instruments SN65HVD230 CAN Adapter
  Usage:  1. Connect ESP32 Board to USB power Source
          2. Touch GPIO 15 (Touch3) with your finger to wake-up the ESP32 from deep-sleep
          3. ESP32 will send a CAN Message to the Lolin Board
          4. This will wake up the Lolin and toggle the LED attached to the Lolin Board
*******************************************************************/
#include "src/ESP32CAN.h"
#include "src/CAN_config.h"
///////////////////////////
///THIS//IS//ON//THE//ESP//
///////////////////////////
CAN_device_t CAN_cfg;
//////////////////////////////////////
#define Threshold 55 /* Greater the value, more the sensitivity */
RTC_DATA_ATTR int bootCount = 0;
bool bfirst_boot = true;
/////////////////////////////////////
void callback(){
}
void setup() {
    Serial.begin(115200);
    ++bootCount;
    delay(3000);
    touchAttachInterrupt(T3, callback, Threshold);
    Serial.println("bcount "+ String(bootCount));
    esp_sleep_enable_touchpad_wakeup();
    if(bootCount <2){
      esp_deep_sleep_start();
    }
    //////////////////CAN Setup
    CAN_cfg.speed=CAN_SPEED_1000KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //start CAN Module
    ESP32Can.CANInit();
    Serial.println("Init finished");
}

void loop() {
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("New standard frame");
      else
        printf("New extended frame");
      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          printf("%c\t", (char)rx_frame.data.u8[i]);
        }
        printf("\n");
      }
    }
    else
    {
      rx_frame.FIR.B.FF = CAN_frame_std;
      rx_frame.MsgID = 1; //set id
      //rx_frame.FIR.B.DLC = 8; //maximal data frame
      rx_frame.FIR.B.DLC = 1;
      rx_frame.data.u8[0] = 't';
      ESP32Can.CANWriteFrame(&rx_frame);
    }
      delay(200); //bisschen delay vor dem sleep nötig
      Serial.println("I sleep");
      esp_deep_sleep_start();
}

#include <ESP32CAN.h>
#include <CAN_config.h>
///////////////////////////
///THIS//IS//ON//THE//ESP//
///////////////////////////


CAN_device_t CAN_cfg;
//////////////////////////////////////
#define Threshold 55 /* Greater the value, more the sensitivity */
RTC_DATA_ATTR int bootCount = 0;
bool bfirst_boot = true;
/////////////////////////////////////
hw_timer_t *My_timer = NULL;
int timercount = 0;
int waittime = 10;
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
    //////////////////Timer Setup

    //////////////////
    //touchDetachInterrupt(T3); //!!! damit nicht ausversehen erneut ausgelöst
    //////////////////
    Serial.println("Init finished");
}

void loop() {
    timercount = 0; //reset bc new request detected
    //
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

      //do stuff!
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
      rx_frame.MsgID = 1; ///das ist die msg id, "from...."
      rx_frame.FIR.B.DLC = 8; //legt das die frame größe fest, oder ist das immer 8 und die anderen sind leer?
      rx_frame.data.u8[0] = 'h';
      rx_frame.data.u8[1] = 'e';
      rx_frame.data.u8[2] = 'l';
      rx_frame.data.u8[3] = 'l';
      rx_frame.data.u8[4] = 'o';
      rx_frame.data.u8[5] = 'c';
      rx_frame.data.u8[6] = 'a';
      rx_frame.data.u8[7] = 'n';

      
      ESP32Can.CANWriteFrame(&rx_frame);
      delay(400);
      Serial.println("I sleep");
      esp_deep_sleep_start();
      //delay(400); //mb no delay? was 1k
    }
}

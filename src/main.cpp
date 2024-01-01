// #include "main.h"
#include <Arduino.h>
#include "./lcd/LCDMLdisp.h"
#include "./flash/rsem.h"
#include <STM32FreeRTOS.h>
#include "loadcell/loadcell.h"


extern SettingStruct settings;
extern InputStruct inputs;
extern LoadCellStruct loadCell;
extern HX711 scale;

const int LOADCELL_DOUT_PIN = PB0;
const int LOADCELL_SCK_PIN = PB1;


SemaphoreHandle_t xSemaphore = NULL;
TaskHandle_t lcdHandle,readInputsHandle,readLoadCellHandle = NULL;


static void lcd(void* arg) {
  UNUSED(arg);
  xSemaphore = xSemaphoreCreateMutex();
  lcd_setup();

  while (1)
  {
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 20 ) == pdTRUE ){
      lcd_loop();
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}




static void readInputs(void* arg) {
  UNUSED(arg);
  xSemaphore = xSemaphoreCreateMutex();
  while (1)
  {
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 20 ) == pdTRUE ){
      inputs.input1 = digitalRead(PA11);
      inputs.input2 = digitalRead(PA12);
      if (inputs.input1 == 0) { digitalWrite(PA6,HIGH);}
      else { digitalWrite(PA6,LOW);}
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}




static void readLoadCell(void* arg) {
  UNUSED(arg);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(settings.scaleVal);                    
  scale.tare();
  while (1)
  {
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 20 ) == pdTRUE ){
      scale.power_up();
      loadCell.value = scale.get_units(15);
      scale.power_down();
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}


void setup() {
  memoryInit();
  xSemaphore = xSemaphoreCreateMutex();
  pinMode(PA6,OUTPUT);
  pinMode(PB2,OUTPUT);
  portBASE_TYPE s1, s2, s3;

  if (xSemaphore != NULL) {

    s1 = xTaskCreate(lcd, "LCD", 512, NULL, 500, &lcdHandle);
    s2 = xTaskCreate(readInputs, "readInputs", 1024, NULL, -1, &readInputsHandle);
    s3 = xTaskCreate(readLoadCell, "readLoadCell", 512, NULL, 400, &readLoadCellHandle);


    if ( s1 != pdPASS|| s2 != pdPASS|| s3 != pdPASS) {
      Serial.println(F("Creation problem"));
      while(1);
    }
    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
  }
  return ;
  //loadDefaultSetting();
  // settings.scaleVal = 100;
  //settings.ratio = 0;
  // settings.coefficent = 1;
  // settings.Hysteresis = 0;
  //settings.password = 111;
  // settings.setPointHigh = 200;
  // settings.setPointLow = 0;
  // settings.unit = 0;
  //memoryWriteSetting();
  //memoryReadSetting();
}

void loop() {
}

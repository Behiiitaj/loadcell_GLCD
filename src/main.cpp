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
TaskHandle_t lcdHandle,readInputsHandle,readLoadCellHandle,loadcellAnalysisHandle = NULL;


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
  delay(200);
  scale.set_scale(settings.scaleVal);  
  scale.power_up();
  scale.tare();
  scale.power_down();
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


static void loadcellAnalysis(void* arg) {
  UNUSED(arg);
  pinMode(PB2,OUTPUT);
  pinMode(PB7,OUTPUT);
  while (1)
  {
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 20 ) == pdTRUE ){
      if (loadCell.setPointActive)
      {
        if (loadCell.value <= settings.setPointLow)
        {
          loadCell.highSetPoint = false;
          loadCell.lowSetPoint = true;
          digitalWrite(PB7,HIGH);
          digitalWrite(PB2,LOW);
        }
        else if (loadCell.value > settings.setPointLow && loadCell.value <= settings.setPointHigh)
        {
          loadCell.highSetPoint = true;
          loadCell.lowSetPoint = false;
          digitalWrite(PB7,LOW);
          digitalWrite(PB2,HIGH);
        }
        else
        {
          loadCell.setPointActive = false;
          loadCell.highSetPoint = false;
          loadCell.lowSetPoint = false;
          digitalWrite(PB7,LOW);
          digitalWrite(PB2,LOW);  
        }

      }
      else
      {
        loadCell.setPointActive = false;
          loadCell.highSetPoint = false;
          loadCell.lowSetPoint = false;
          digitalWrite(PB7,LOW);
          digitalWrite(PB2,LOW);
      }

      
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}


void setup() {
  memoryInit();
  xSemaphore = xSemaphoreCreateMutex();
  portBASE_TYPE s1, s2, s3, s4;

  // loadDefaultSetting();
  // settings.password = 000;
  // settings.scaleVal = 100000;
  // settings.ratio = 1;
  // settings.coefficent = 1;
  // settings.Hysteresis = 0;
  // settings.setPointHigh = 8000;
  // settings.setPointLow = 3000;
  // settings.unit = 0;
  // memoryReadSetting();
  // memoryWriteSetting();
  if (xSemaphore != NULL) {

    s1 = xTaskCreate(lcd, "LCD", 512, NULL, 500, &lcdHandle);
    s2 = xTaskCreate(readInputs, "readInputs", 512, NULL, -1, &readInputsHandle);
    s3 = xTaskCreate(readLoadCell, "readLoadCell", 512, NULL, 400, &readLoadCellHandle);
    s4 = xTaskCreate(loadcellAnalysis, "loadcellAnalysis", 512, NULL, 300, &loadcellAnalysisHandle);

    if ( s1 != pdPASS|| s2 != pdPASS|| s3 != pdPASS || s4 != pdPASS) {
      Serial.println(F("Creation problem"));
      while(1);
    }
    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
  }
  return ;
}

void loop() {
}

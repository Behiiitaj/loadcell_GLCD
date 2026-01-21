// #include "main.h"
#include <Arduino.h>
#include "./lcd/LCDMLdisp.h"
#include "./flash/rsem.h"
#include <STM32FreeRTOS.h>
#include "loadcell/loadcell.h"
#include <ezBuzzer.h>

ezBuzzer buzzer(PB7);

extern SettingStruct settings;
extern InputStruct inputs;
extern LoadCellStruct loadCell;
extern HX711 scale;
extern FunctionStruct functions;

const int LOADCELL_DOUT_PIN = PB0;
const int LOADCELL_SCK_PIN = PB1;

SemaphoreHandle_t xSemaphore = NULL;
TaskHandle_t lcdHandle, readInputsHandle, readLoadCellHandle, loadcellAnalysisHandle = NULL;

static void lcd(void *arg)
{
  UNUSED(arg);
  xSemaphore = xSemaphoreCreateMutex();
  lcd_setup();

  while (1)
  {
    if (xSemaphoreTake(xSemaphore, (TickType_t)20) == pdTRUE)
    {
      lcd_loop();
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}

static void readInputs(void *arg)
{
  UNUSED(arg);
  xSemaphore = xSemaphoreCreateMutex();
  while (1)
  {
    if (xSemaphoreTake(xSemaphore, (TickType_t)20) == pdTRUE)
    {
      inputs.input1 = digitalRead(PA11);
      inputs.input2 = digitalRead(PA12);
      if (inputs.input1 == 0)
        // buzzer.beep(75, 50);
        functions.functionActive = true;
      if (inputs.input2 == 0)
        // buzzer.beep(75, 50);
        functions.functionActive = false;
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}

static void readLoadCell(void *arg)
{
  UNUSED(arg);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);
  delay(200);
  scale.set_scale(settings.scaleVal);
  scale.power_up();
  if (settings.startupTare)
    scale.tare();
  else
    scale.set_offset(settings.deadTare);
  scale.power_down();
  while (1)
  {
    if (xSemaphoreTake(xSemaphore, (TickType_t)20) == pdTRUE)
    {
      getWeight();
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
  }
}

int dosingPumpBeep = 10;

static void loadcellAnalysis(void *arg)
{
  UNUSED(arg);
  digitalWrite(PB2, HIGH);
  pinMode(PB2, OUTPUT);

  digitalWrite(PB10, HIGH);
  pinMode(PB10, OUTPUT);

  bool firstLOW = true;
  bool firstHIGH = true;
  while (1)
  {
    buzzer.loop();
    if (settings.currentFunction == 0)
    {
      if (functions.functionActive)
      {
        if (loadCell.value <= settings.setPointLow)
        {
          if (firstLOW)
          {
            firstLOW = false;
            firstHIGH = true;
            buzzer.beep(75, 100);
            buzzer.beep(75);
          }
          functions.highSetPoint = false;
          functions.lowSetPoint = true;
          digitalWrite(PB2, LOW);
          digitalWrite(PB10, HIGH);
        }
        else if (loadCell.value > settings.setPointLow && loadCell.value <= settings.setPointHigh)
        {
          if (firstHIGH)
          {
            buzzer.beep(75, 50);
            firstLOW = true;
            buzzer.beep(75, 50);
            firstHIGH = false;
            buzzer.beep(75);
          }
          functions.highSetPoint = true;
          functions.lowSetPoint = false;
          digitalWrite(PB2, HIGH);
          digitalWrite(PB10, LOW);
        }
        else
        {
          if (functions.highSetPoint && !functions.lowSetPoint)
          {
            buzzer.beep(600, 200);
            firstLOW = true;
            buzzer.beep(1000);
            firstHIGH = true;
          }
          functions.functionActive = false;
          functions.highSetPoint = false;
          functions.lowSetPoint = false;
          digitalWrite(PB2, HIGH);
          digitalWrite(PB10, HIGH);
        }
      }
      else
      {
        functions.functionActive = false;
        functions.highSetPoint = false;
        functions.lowSetPoint = false;
        digitalWrite(PB2, HIGH);
        digitalWrite(PB10, HIGH);
      }
    }
    //--> Dosing pump
    else if (settings.currentFunction == 1)
    {
      if (functions.functionActive)
      {
        int dosingOutputNumber, fullTimeoutputNumber;
        if (settings.dosingFunction.outputNumber == 0)
        {
          dosingOutputNumber = PB2;
          fullTimeoutputNumber = PB10;
        }
        else if (settings.dosingFunction.outputNumber == 1)
        {
          dosingOutputNumber = PB10;
          fullTimeoutputNumber = PB2;
        }
        digitalWrite(fullTimeoutputNumber, LOW);
        if (settings.dosingFunction.lowTime < dosingPumpBeep)
          dosingPumpBeep = settings.dosingFunction.lowTime;
        int highTimeSlice = 0;
        int lowTimeSlice = 0;
        for (int i = 0; i < settings.dosingFunction.cycleCount; i++)
        {
          highTimeSlice = 0;
          lowTimeSlice = 0;
          if (functions.functionActive)
          {
            functions.currentCounterNumber = i + 1;
            functions.dosingHigh = true;
            digitalWrite(dosingOutputNumber, LOW);
            while (highTimeSlice < settings.dosingFunction.highTime)
            {
              if (!functions.functionActive)
              {
                functions.dosingHigh = false;
                break;
              }
              highTimeSlice += 10;
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            functions.dosingHigh = false;
            if (!functions.functionActive)
            {
              break;
            }
            digitalWrite(dosingOutputNumber, HIGH);
            digitalWrite(PB7, HIGH);
            vTaskDelay(dosingPumpBeep);
            digitalWrite(PB7, LOW);
            while (lowTimeSlice < settings.dosingFunction.lowTime - dosingPumpBeep)
            {
              if (!functions.functionActive)
              {
                break;
              }
              lowTimeSlice += 10;
              vTaskDelay(pdMS_TO_TICKS(10));
            }
          }
        }

        functions.keepAliveCounter = settings.dosingFunction.lastTime;
        functions.keepAlive = true;
        int waitTime = 0;
        int keepAlive = settings.dosingFunction.lastTime * 1000;
        int counterForOneSec = 0;

        while (functions.functionActive && ((waitTime * 100) < keepAlive))
        {
          if (counterForOneSec == 10)
          {
            counterForOneSec = 0;
            functions.keepAliveCounter--;
          }
          vTaskDelay(pdMS_TO_TICKS(100));
          waitTime++;
          counterForOneSec++;
        }
        functions.functionActive = false;
        functions.keepAlive = false;
        digitalWrite(dosingOutputNumber, HIGH);
        digitalWrite(fullTimeoutputNumber, HIGH);

        digitalWrite(PB7, HIGH);
        vTaskDelay(500);
        digitalWrite(PB7, LOW);
        vTaskDelay(400);
        digitalWrite(PB7, HIGH);
        vTaskDelay(500);
        digitalWrite(PB7, LOW);
        vTaskDelay(400);
        digitalWrite(PB7, HIGH);
        vTaskDelay(1200);
        digitalWrite(PB7, LOW);
      }
    }
    vTaskDelay(1);
  }
}

void setup()
{
  memoryInit();
  xSemaphore = xSemaphoreCreateMutex();
  portBASE_TYPE s1, s2, s3, s4;

  // loadDefaultSetting();
  // memoryWriteSetting();
  if (xSemaphore != NULL)
  {

    s1 = xTaskCreate(lcd, "LCD", 512, NULL, 500, &lcdHandle);
    s2 = xTaskCreate(readInputs, "readInputs", 512, NULL, -1, &readInputsHandle);
    s3 = xTaskCreate(readLoadCell, "readLoadCell", 512, NULL, 400, &readLoadCellHandle);
    s4 = xTaskCreate(loadcellAnalysis, "loadcellAnalysis", 512, NULL, 300, &loadcellAnalysisHandle);

    if (s1 != pdPASS || s2 != pdPASS || s3 != pdPASS || s4 != pdPASS)
    {
      Serial.println(F("Creation problem"));
      while (1)
        ;
    }
    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
  }
  return;
}

void loop()
{
}

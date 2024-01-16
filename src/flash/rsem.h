#ifndef MEM_H
#define MEM_H

#include <Arduino.h>
#include <EEPROM.h>



#define DEFAULT_SETTING_SAVED_ID                      0x5100


typedef struct {
  uint16_t saved;
  uint32_t uniqId[4];

  float scaleVal;
  int ratio;
  double coefficent;
  uint16_t Hysteresis;
  uint16_t password;
  int unit;
  int setPointHigh;
  int setPointLow;
  int zeroFilter;
  int avgCount;
} SettingStruct;


typedef struct {
  int input1;
  int input2;
} InputStruct;


typedef struct {
  float value;
  float pureADC;
  bool highSetPoint;
  bool lowSetPoint;
  bool setPointActive;
  bool isOver;
} LoadCellStruct;

void loadDefaultSetting(void);
void memoryInit(void);
void memoryWriteSetting(void);
void memoryReadSetting(void);

#endif

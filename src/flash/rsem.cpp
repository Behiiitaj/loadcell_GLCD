#include "rsem.h"

SettingStruct settings;
InputStruct inputs;
LoadCellStruct loadCell;


const uint16_t eeAddress = 0;


void loadDefaultSetting(void) {
  settings.scaleVal = 100000;
  settings.ratio = 3;
  settings.coefficent = 1;
  settings.Hysteresis = 0;
  settings.password = 000;
  settings.setPointHigh = 400;
  settings.setPointLow = 100;
  settings.unit = 0;
  settings.avgCount = 5;
  settings.zeroFilter = 1;
}

uint32_t internalFlashRead32bit(uint32_t address) {
  return (*(__IO uint32_t*) address);
}

uint16_t internalFlashRead16bit(uint32_t address) {
  return (*(__IO uint16_t*) address);
}

void internalFlashGetUniqueId (uint32_t *Unique_ID) {
  Unique_ID[0] = internalFlashRead16bit(UID_BASE);
  Unique_ID[1] = internalFlashRead16bit(UID_BASE + 0x02);
  Unique_ID[2] = internalFlashRead32bit(UID_BASE + 0x04);
  Unique_ID[3] = internalFlashRead32bit(UID_BASE + 0x08);
}

void memoryWriteSetting(void) {
  settings.saved = DEFAULT_SETTING_SAVED_ID;
  EEPROM.put(eeAddress, settings);
}

void memoryReadSetting(void) {
  EEPROM.get(eeAddress, settings);
}

void memoryInit(void) {
  internalFlashGetUniqueId(settings.uniqId);//Get Unique chip ID for set MAC Address
  memoryReadSetting();
  
  if(settings.saved != DEFAULT_SETTING_SAVED_ID) {
    loadDefaultSetting();
	  memoryWriteSetting();
  }
  else {
  }
}

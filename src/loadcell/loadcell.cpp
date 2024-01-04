// hx711.cpp
#include "hx711.h"
#include "../flash/rsem.h"
extern SettingStruct settings;

HX711 scale;  // Define the HX711 object

void tareLoadCell(){
    scale.power_up();
    scale.tare();    
    scale.power_down();  
}


void calibrateLoadCell(float weight){
    scale.power_up();
    float newScale = scale.get_units(5) / weight * settings.scaleVal;
    settings.scaleVal = newScale;
    memoryWriteSetting();
    scale.set_scale(settings.scaleVal); 
    scale.power_down();
}
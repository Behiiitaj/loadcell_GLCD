// hx711.cpp
#include "hx711.h"
#include "../flash/rsem.h"
extern SettingStruct settings;
extern LoadCellStruct loadCell;

HX711 scale;  // Define the HX711 object
const float ratioList[7]= { 1000, 100, 10, 1 , 0.1 , 0.01 , 0.001 };

void tareLoadCell(){
    scale.power_up();
    scale.get_units(20);
    scale.tare();    
    scale.power_down();  
}


void calibrateLoadCell(float weight){
    scale.power_up();
    float newScale = scale.get_units(10) / weight * settings.scaleVal;
    settings.scaleVal = newScale;
    memoryWriteSetting();
    scale.set_scale(settings.scaleVal); 
    scale.power_down();
}


void getWeight(){
    scale.power_up();
    // float lastSample = loadCell.value;
    // float resultVal = lastSample;
    // float trueSampleCount = 0;
    // float sample = 0;
    // for (size_t i = 0; i < settings.avgCount; i++)
    // {
    //     sample = scale.get_units(1);
    //     if (abs(sample - lastSample) < 30 )
    //     {
    //         trueSampleCount ++;
    //         resultVal +=sample;
    //     }
    //     lastSample=sample;
    // }
    // if (trueSampleCount>0)
    // {
    //     loadCell.value = resultVal / trueSampleCount; 
    // }
    
    
    loadCell.pureADC = scale.read_average(1);
    if (loadCell.pureADC > 8300000 || loadCell.pureADC < -8300000) loadCell.isOver = true;
    else
    {
        loadCell.isOver = false;
        loadCell.value = ( scale.get_units(settings.avgCount) * ratioList[settings.ratio] * settings.coefficent);
    } 
    scale.power_down();
}
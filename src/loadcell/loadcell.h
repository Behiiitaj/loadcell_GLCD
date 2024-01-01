// hx711.h
#ifndef HX711_H
#define HX711_H

#include <HX711.h>

extern HX711 scale;  // Declare the HX711 object as an external variable

void tareLoadCell();
void calibrateLoadCell(float weight);

#endif  // HX711_H
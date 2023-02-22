#include <Arduino.h>

#include "SensorViews.hpp"
#include "SensorModels.hpp"

HallSensor44eModel hallSensor(SIMPLE_MAGNETIC_MIXER);
//TemperatureSensor18b20Model tempSensor;

void setup(void)
{
    Serial.begin(115200); 
    hallSensor.init();
    //tempSensor.init();
}


void loop(void) 
{
}
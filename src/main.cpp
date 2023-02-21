#include <Arduino.h>
#include "SensorViews.hpp"
#include "SensorModels.hpp"

TemperatureSensor18b20Model tempSensor;
HallSensor44eModel hallSensor(SIMPLE_MAGNETIC_MIXER);

LCD128x64View lcd(tempSensor, hallSensor);
RgbLedView led(tempSensor, hallSensor);

void setup(void)
{
    Serial.begin(115200); 

    hallSensor.init(&led);
    tempSensor.init();
}

void loop(void) 
{
    auto delayTime = 20;
    tempSensor.loopAction(delayTime);
    lcd.loopAction();
    auto delayEnd = led.loopAction();
    delay(delayTime);
    *delayEnd = false;
}
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

    hallSensor.init();
    tempSensor.init();
}

uint8_t delayTime = 20;
uint8_t counterUpdateTemperature = 0;
void loop(void) 
{
    if(++counterUpdateTemperature == (1000u/delayTime))
    {
        tempSensor.loopAction();
        counterUpdateTemperature = 0;
    }
    hallSensor.updateBlink();

    lcd.loopAction(counterUpdateTemperature);
    led.loopAction(!counterUpdateTemperature);
    delay(delayTime);

}
#include "SensorViews.hpp"
#include "SensorModels.hpp"

HallSensor44eModel hallSensor(SIMPLE_MAGNETIC_MIXER);
TemperatureSensor18b20Model tempSensor;

LCD128x64View lcd(tempSensor, hallSensor);
RgbLedView    led(tempSensor, hallSensor);

void setup(void)
{
    Serial.begin(115200); 

    hallSensor.init();
    tempSensor.init();
}

unsigned int counter = 0;
const unsigned int delayTime = 20;

void loop(void) 
{
    hallSensor.updateBlink();

    if(++counter == (1000u / delayTime)){
        tempSensor.loopAction();
        counter = 0;
    }

    lcd.loopAction(counter);
    led.loopAction(!counter);
    
    delay(delayTime);
}
#include "SensorModels.hpp"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "SensorViews.hpp"

#define DEBUG

HallSensor44eModel* HallSensor44eModel::m_instance = nullptr;

HallSensor44eModel::HallSensor44eModel(const uint8_t dipoleMagnetCount) 
    : m_dipoleMagnetCount(dipoleMagnetCount) {}

void HallSensor44eModel::init()
{
    HallSensor44eModel::m_instance = this;

    pinMode(m_pinBoardLed, OUTPUT);
    pinMode(m_pinInterrupt, INPUT);

    TaskHandle_t taskHandle;
    if(auto result = xTaskCreate(taskUpdateBlink, "hsub", configMINIMAL_STACK_SIZE, this, tskIDLE_PRIORITY + 1, &taskHandle)){
        if(result){
            #ifdef DEBUG
                Serial.println("Error: not created task updateBlink. Result: " + String(result));
            #endif
            return;
        } 
    }

    m_interruptSemaphore = xSemaphoreCreateBinary();
    if(m_interruptSemaphore){
        attachInterrupt(m_pinInterrupt - 2, interrupt, CHANGE);
    }

    #ifdef DEBUG
        Serial.println("Class HallSensor44eModel was initialized.");
    #endif
}

void HallSensor44eModel::updateBlink()
{
    if(m_isUpdatedBlink) {
        digitalWrite(m_pinBoardLed, HIGH);
        m_isUpdatedBlink = false;
    }
    else {
        digitalWrite(m_pinBoardLed, LOW);
    }
}

void HallSensor44eModel::taskUpdateBlink(void* arg)
{
    auto instance = reinterpret_cast<HallSensor44eModel*>(arg);
    while(true) {
        if(xSemaphoreTake(instance->m_interruptSemaphore, portMAX_DELAY) == pdPASS) {
            instance->updateBlink();
        }
        vTaskDelay(50u / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void HallSensor44eModel::interrupt()
{
    uint16_t delta = millis() - m_instance->m_debounce;
    if (delta >= 50 && digitalRead(m_instance->m_pinInterrupt))  {
        memcpy(&m_instance->m_rotationTimes[0], 
               &m_instance->m_rotationTimes[1], 
               (SIZE_ROTATION_TIMES - 1) * sizeof(uint16_t));

        m_instance->m_rotationTimes[SIZE_ROTATION_TIMES - 1] = delta;
        m_instance->m_debounce = millis();

        m_instance->m_isUpdatedBlink = true;
        xSemaphoreGiveFromISR(m_instance->m_interruptSemaphore, NULL);

    #ifdef DEBUG
        Serial.println("Hall sensor indicate rotation time: " + String(delta));
    #endif
    }
}

uint16_t HallSensor44eModel::getLastRotationTime() const
{
    return m_rotationTimes[SIZE_ROTATION_TIMES - 1];
}

uint16_t HallSensor44eModel::getAverageRotationTime() const
{
    const uint16_t maxRotationTime = 4000;
    uint8_t counter = 0;
    uint16_t average = 0;

    #ifdef DEBUG
        String text;
    #endif

    for(auto value: m_rotationTimes) {
        if(value != 0 && value < maxRotationTime) {
            average += value;
            ++counter;
        }

        #ifdef DEBUG
            text += "{" + String(value) + "}";
        #endif
    }

    average = (average / counter) / m_dipoleMagnetCount;
    if(average > maxRotationTime)
        average = 0;

    #ifdef DEBUG
        Serial.println("Average: " + String(average) + " Rotation times: " + text);
    #endif
    return average;
}

TemperatureSensor18b20Model::~TemperatureSensor18b20Model()
{
    if(m_oneWire)
        delete m_oneWire;
    if(m_dallasTemperature)
        delete m_dallasTemperature;
}

void TemperatureSensor18b20Model::init()
{
    m_oneWire = new OneWire(m_pin);
    m_dallasTemperature = new DallasTemperature(m_oneWire);

    m_dallasTemperature->begin();

    TaskHandle_t taskHandle;
    if(auto result = xTaskCreate(taskLoop, "tstl", configMINIMAL_STACK_SIZE, this, tskIDLE_PRIORITY + 1, &taskHandle)){
        if(result){
            #ifdef DEBUG
                Serial.println("Error: not created task updateBlink. Result: " + String(result));
            #endif
            return;
        } 
    }

    #ifdef DEBUG
        Serial.println("Class TemperatureSensor18b20Model was initialized.");
    #endif
}

void TemperatureSensor18b20Model::loopAction()
{
    m_dallasTemperature->requestTemperatures();
    m_currentTemp = m_dallasTemperature->getTempCByIndex(0);

    #ifdef DEBUG
        Serial.println("Current temperature: " + String(m_currentTemp));
    #endif

    if(m_currentTemp <= -127.00)
        return;

    memcpy(&m_tempTimes[0], 
           &m_tempTimes[1], 
           (SIZE_TEMPERATURE_TIMES - 1) * sizeof(float));

    m_tempTimes[SIZE_TEMPERATURE_TIMES - 1] = m_currentTemp;

    m_currentAverageTemp = getAverageTemperature();
}

void TemperatureSensor18b20Model::taskLoop(void* arg)
{
    auto instance = reinterpret_cast<TemperatureSensor18b20Model*>(arg);
    while(true) {
        instance->loopAction();
        vTaskDelay(1000u / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

float TemperatureSensor18b20Model::getLastTemperature() const
{
    return m_currentTemp;
}

float TemperatureSensor18b20Model::getLastAverageTemperature() const
{
    return m_currentAverageTemp;
}

float TemperatureSensor18b20Model::getAverageTemperature() const
{
    uint8_t counter = 0;
    float average = 0.0;

    #ifdef DEBUG
        String text;
    #endif

    for(auto value: m_tempTimes) {
        if(value != 0) {
            average += value;
            ++counter;
        }

        #ifdef DEBUG
            text += "{" + String(value) + "}";
        #endif
    }

    #ifdef DEBUG
        Serial.println("Average temp: " + String(average) + " : " + text);
    #endif

    if(average == 0.0){
        return 0.0;
    }
    return average / counter;
}
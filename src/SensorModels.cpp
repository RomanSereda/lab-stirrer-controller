#include "SensorModels.hpp"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "SensorViews.hpp"

//#define DEBUG

HallSensor44eModel* HallSensor44eModel::m_instance = nullptr;

HallSensor44eModel::HallSensor44eModel(const uint8_t dipoleMagnetCount) 
    : m_dipoleMagnetCount(dipoleMagnetCount) {}

void HallSensor44eModel::init()
{
    HallSensor44eModel::m_instance = this;

    pinMode(m_pinBoardLed, OUTPUT);
    pinMode(m_pinInterrupt, INPUT);

    attachInterrupt(m_pinInterrupt - 2, interrupt, CHANGE);

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

void HallSensor44eModel::interrupt()
{
    uint16_t delta = millis() - m_instance->m_debounce;
    if (delta >= 50 && digitalRead(m_instance->m_pinInterrupt))  {
        memcpy(&m_instance->m_rotationTimes[0], 
               &m_instance->m_rotationTimes[1], 
               (SIZE_ROTATION_TIMES - 1) * sizeof(uint16_t));

        m_instance->m_currentRtTime = delta;
        m_instance->m_rotationTimes[SIZE_ROTATION_TIMES - 1] = delta;
        
        m_instance->m_debounce = millis();
        m_instance->m_isUpdatedBlink = true;

        m_instance->m_currentAverageRpm = m_instance->getAverageRpm();
    }
}

uint16_t HallSensor44eModel::getLastRotationTime() const
{
    return m_currentRtTime;
}

float HallSensor44eModel::getLastAverageRotationTime() const
{
    return m_currentAverageRpm;
}

float HallSensor44eModel::getAverageRpm() const
{
    const uint16_t maxRotationTime = 4000;
    uint8_t counter = 0;
    float average = 0.0f;

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

    #ifdef DEBUG
        Serial.println("Rotation avg time: " + String(average) + " r-avg items: " + text);
    #endif

    if(average == 0.0f)
        return 0.0f;

    average = (average / counter) / m_dipoleMagnetCount;
    if(average > maxRotationTime)
        return 0.0f;

    return 1000.0f / average;
}

TemperatureSensor18b20Model::~TemperatureSensor18b20Model()
{
    if(m_oneWire)
        delete m_oneWire;
    if(m_dallasTemperature)
        delete m_dallasTemperature;
    if(m_tempDiff)
        delete m_tempDiff;
}

void TemperatureSensor18b20Model::init()
{
    m_oneWire = new OneWire(m_pin);
    m_dallasTemperature = new DallasTemperature(m_oneWire);
    m_tempDiff = new TempDiff;

    m_dallasTemperature->begin();

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

    if(m_currentTemp <= -127.00f)
        return;

    memcpy(&m_tempTimes[0], 
           &m_tempTimes[1], 
           (SIZE_TEMPERATURE_TIMES - 1) * sizeof(float));

    m_tempTimes[SIZE_TEMPERATURE_TIMES - 1] = m_currentTemp;

    m_currentDiffTemp = getDiffTemperature();

    #ifdef DEBUG
        Serial.println("Diff temp: " + String(m_currentDiffTemp));
    #endif
}

float TemperatureSensor18b20Model::getLastTemperature() const
{
    return m_currentTemp;
}

float TemperatureSensor18b20Model::getLastDiffTemperature() const
{
    return m_currentDiffTemp;
}

TempDiff* TemperatureSensor18b20Model::getTempDiff()
{
    return m_tempDiff;
}

float TemperatureSensor18b20Model::getDiffTemperature() const
{
    #ifdef DEBUG
        String text;
        for(auto value: m_tempTimes) {
            text += "{" + String(value) + "}";
        }
        Serial.println("Temp times: " + text);
    #endif

    float negDiff = 0.0f;
    float posDiff = 0.0f;

    for (size_t i = 1; i < SIZE_TEMPERATURE_TIMES - 1; i++)
    {
        if(m_tempTimes[i - 1] == 0.0)
            continue;

        auto diff = m_tempTimes[i] - m_tempTimes[i - 1];
        if(diff > 0)
            posDiff += diff;
        else
            negDiff += diff;
    }

    #ifdef DEBUG
        Serial.println("posDiff: " + String(posDiff) + " negDiff: " + String(negDiff));
    #endif

    m_tempDiff->neg = negDiff;
    m_tempDiff->pos = posDiff;

    return negDiff + posDiff;
}
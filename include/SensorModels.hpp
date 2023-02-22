#pragma once
#include <Arduino.h>
#define SIMPLE_MAGNETIC_MIXER 1
#define SIZE_ROTATION_TIMES 4
#define SIZE_TEMPERATURE_TIMES 8
#define MAX_TEMPERATURE 120

class OneWire;
class DallasTemperature;
class RgbLedView;

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

class HallSensor44eModel
{
public:
    HallSensor44eModel(const uint8_t dipoleMagnetCount);
    void init();
    
    uint16_t getLastRotationTime() const;
    uint16_t getAverageRotationTime() const;

private:
    const uint8_t m_pinInterrupt = 2;
    const uint8_t m_pinBoardLed = 13;

    volatile bool m_isUpdatedBlink = false;
    void updateBlink();

    SemaphoreHandle_t m_interruptSemaphore;
    static void taskUpdateBlink(void* arg);

    volatile uint16_t m_debounce = 0;
    const uint8_t m_dipoleMagnetCount;
    
    static HallSensor44eModel* m_instance;
    static void interrupt();
    
    uint16_t m_rotationTimes[SIZE_ROTATION_TIMES] = {0};
};

class TemperatureSensor18b20Model 
{
public:
    virtual ~TemperatureSensor18b20Model();
    void init();

    float getLastTemperature() const;
    float getLastAverageTemperature() const;

private:
    OneWire* m_oneWire = nullptr;
    DallasTemperature* m_dallasTemperature = nullptr;

    static void taskLoop(void* arg);
    void loopAction();

    float getAverageTemperature() const;

    const uint8_t m_pin = 4;
    float m_tempTimes[SIZE_TEMPERATURE_TIMES] = {0.0};
    unsigned int m_counter = 0;

    float m_currentTemp = 0.0;
    float m_currentAverageTemp = 0.0;
};
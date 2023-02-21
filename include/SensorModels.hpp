#pragma once
#include <Arduino.h>
#define SIMPLE_MAGNETIC_MIXER 1
#define SIZE_ROTATION_TIMES 4
#define SIZE_TEMPERATURE_TIMES 8
#define MAX_TEMPERATURE 120

class OneWire;
class DallasTemperature;
class RgbLedView;

class HallSensor44eModel
{
public:
    HallSensor44eModel(const uint8_t dipoleMagnetCount);
    void init(RgbLedView* rgbLed);

    uint16_t getLastRotationTime() const;
    uint16_t getAverageRotationTime() const;

private:
    const uint8_t m_dipoleMagnetCount;
    const uint8_t m_pinInterrupt = 2;
    volatile uint16_t m_debounce = 0;

    static void interrupt();
    uint16_t m_rotationTimes[SIZE_ROTATION_TIMES] = {0};

    static HallSensor44eModel* m_instance;
    RgbLedView* m_rgbLed = nullptr;
};

class TemperatureSensor18b20Model 
{
public:
    virtual ~TemperatureSensor18b20Model();
    void init();

    void loopAction(int delayTime);

    float getLastTemperature() const;
    float getAverageTemperature() const;

private:
    OneWire* m_oneWire = nullptr;
    DallasTemperature* m_dallasTemperature = nullptr;

    const uint8_t m_pin = 4;
    float m_tempTimes[SIZE_TEMPERATURE_TIMES] = {0.0};
    unsigned int m_counter = 0;
    float m_currentTemp = 0.0;
};
#pragma once
#include <Arduino.h>
#define SIMPLE_MAGNETIC_MIXER 1
#define SIZE_ROTATION_TIMES 4
#define SIZE_TEMPERATURE_TIMES 8

class OneWire;
class DallasTemperature;
class RgbLedView;

struct TempDiff
{
    float neg;
    float pos;
};

class HallSensor44eModel
{
public:
    HallSensor44eModel(const uint8_t dipoleMagnetCount);
    void init();

    void updateBlink();
    
    uint16_t getLastRotationTime() const;
    float getLastAverageRotationTime() const;

private:
    const uint8_t m_pinInterrupt = 2;
    const uint8_t m_pinBoardLed = 13;

    volatile uint16_t m_debounce = 0;
    const uint8_t m_dipoleMagnetCount;

    
    volatile bool m_isUpdatedBlink = false;
    
    static HallSensor44eModel* m_instance;
    static void interrupt();
    
    uint16_t m_rotationTimes[SIZE_ROTATION_TIMES] = {0};

    uint16_t m_currentRtTime = 0;
    float m_currentAverageRpm = 0.0;

    float getAverageRpm() const;
};

class TemperatureSensor18b20Model 
{
public:
    virtual ~TemperatureSensor18b20Model();
    void init();

    void loopAction();

    float getLastTemperature() const;
    float getLastDiffTemperature() const;

    TempDiff* getTempDiff();

private:
    OneWire* m_oneWire = nullptr;
    DallasTemperature* m_dallasTemperature = nullptr;

    const uint8_t m_pin = 4;
    float m_tempTimes[SIZE_TEMPERATURE_TIMES] = {0.0};

    float m_currentTemp = 0.0;
    float m_currentDiffTemp = 0.0;

    TempDiff* m_tempDiff;
    float getDiffTemperature() const;
};
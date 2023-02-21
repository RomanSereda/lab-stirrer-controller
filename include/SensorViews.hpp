#pragma once
#include <Arduino.h>
#include "U8glib.h"

class HallSensor44eModel;
class TemperatureSensor18b20Model;

class LCD128x64View
{
public:
    explicit LCD128x64View(TemperatureSensor18b20Model& tempSensor, HallSensor44eModel& hallSensor);
    void loopAction();

private:
    const uint8_t m_pinD0  = 12; //CKS
    const uint8_t m_pinD1  = 11; //MOSI
    const uint8_t m_pinRES = 10;
    const uint8_t m_pinDC  = 9;  //A0
    const uint8_t m_pinCS  = 8;

    TemperatureSensor18b20Model& m_tempSensor;
    HallSensor44eModel& m_hallSensor;

    U8GLIB_SSD1306_128X64 m_u8g;
};

class RgbLedView
{
public:
    RgbLedView(TemperatureSensor18b20Model& tempSensor, HallSensor44eModel& hallSensor);

    volatile bool* loopAction();
    void setHallSensorFlaf();

private:
    const uint8_t m_pinRed   = 3;
    const uint8_t m_pinGreed = 5;
    const uint8_t m_pinBlue  = 6;

    volatile bool m_hallSignalFlag = false;

    TemperatureSensor18b20Model& m_tempSensor;
    HallSensor44eModel& m_hallSensor;

    void setRGB(uint8_t r, uint8_t g, uint8_t b);
};
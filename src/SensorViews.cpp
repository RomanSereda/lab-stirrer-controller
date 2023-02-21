#include "SensorViews.hpp"
#include "SensorModels.hpp"

//#define DEBUG
#define OFF_COMMON_ANODE 255
#define ON_COMMON_ANODE 0

LCD128x64View::LCD128x64View(TemperatureSensor18b20Model& tempSensor, HallSensor44eModel& hallSensor)
    : m_tempSensor(tempSensor), m_hallSensor(hallSensor), 
      m_u8g(U8GLIB_SSD1306_128X64(m_pinD0, m_pinD1, m_pinCS, m_pinDC, m_pinRES)) 
{
    m_u8g.setFont(u8g_font_unifont);
}

void LCD128x64View::loopAction()
{
    m_u8g.firstPage();
    do
    {
        String line1 = "T:" + String(m_tempSensor.getLastTemperature()) 
                     + " (" + String(m_tempSensor.getAverageTemperature()) + ")";

        auto rt = m_hallSensor.getLastRotationTime();
        String line2 = "Rt: "+ String(1000u / rt) 
                     + "rpm (" + String(rt) + "ms)";

        m_u8g.drawStr(0, 28, line1.c_str());
        m_u8g.drawStr(0, 48, line2.c_str());
    } while (m_u8g.nextPage());
}

RgbLedView::RgbLedView(TemperatureSensor18b20Model& tempSensor, HallSensor44eModel& hallSensor)
    : m_tempSensor(tempSensor), m_hallSensor(hallSensor) 
{
    pinMode(m_pinRed, OUTPUT);
    pinMode(m_pinGreed, OUTPUT);
    pinMode(m_pinBlue, OUTPUT);

    setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, OFF_COMMON_ANODE);
}

volatile bool* RgbLedView::loopAction()
{
    auto time = m_hallSensor.getLastRotationTime();
    if(m_hallSignalFlag && time > 1000)
    {
        if(!time)
        {
            setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, OFF_COMMON_ANODE);
            return &m_hallSignalFlag;
        }

        setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, ON_COMMON_ANODE);
    }
    else
    {
        auto value = m_tempSensor.getLastTemperature();
        if(!value)
        {
            setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, OFF_COMMON_ANODE);
            return &m_hallSignalFlag;
        }

        auto iv = static_cast<uint8_t>(value * 4);
        setRGB(255 - iv, iv, OFF_COMMON_ANODE);

    }
    return &m_hallSignalFlag;
}

void RgbLedView::setHallSensorFlaf()
{
    m_hallSignalFlag = true;
}

void RgbLedView::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(m_pinBlue, b);
    analogWrite(m_pinRed, r);
    analogWrite(m_pinGreed, g);    
}

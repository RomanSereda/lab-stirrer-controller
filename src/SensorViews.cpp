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

void LCD128x64View::loopAction(uint8_t tick)
{
    m_u8g.firstPage();
    do {
        String line1 = "T:" + String(m_tempSensor.getLastTemperature()) 
                     + " (" + String(m_tempSensor.getLastAverageTemperature()) + ")";

        auto rt = m_hallSensor.getLastRotationTime();
        String line2 = "Rt: "+ String(1000.0f / rt) 
                     + "rpm (" + String(rt) + "ms)";

        String strTick;
        for (size_t i = 0; i < tick; i++)
            strTick += ".";
        
        m_u8g.drawStr(0, 3,  strTick.c_str());
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

    offRGB();
}

void RgbLedView::loopAction(bool isBlueBlink)
{
    if(isBlueBlink)
    {
        setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, ON_COMMON_ANODE);
    }
    else
    {
        auto value = m_tempSensor.getLastTemperature();
        if(!value)
        {
            offRGB();
            return;
        }
        auto iv = static_cast<uint8_t>(value * 4);
        setRGB(255 - iv, iv, OFF_COMMON_ANODE);
    }
}

void RgbLedView::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(m_pinBlue, b);
    analogWrite(m_pinRed, r);
    analogWrite(m_pinGreed, g);    
}

void RgbLedView::offRGB()
{
    setRGB(OFF_COMMON_ANODE, OFF_COMMON_ANODE, OFF_COMMON_ANODE);
}

#include <stdexcept>
#include <cmath>

#include <iostream>

#include "MiLightPilot.h"


using namespace device;
using namespace milight;
using namespace std;

#ifndef SYMULATOR
MiLightPilot::MiLightPilot()
	:	RF24_(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_1MHZ),
		PL1167_nRF24_(RF24_),
		radio_(PL1167_nRF24_)
{

}
#else
void delay(unsigned int) { }

MiLightPilot::MiLightPilot()
{

}
#endif

MiLightPilot::~MiLightPilot()
{

}

void MiLightPilot::Init()
{
    radio_.begin();
}

void MiLightPilot::SendCmd(CMDS cmd, bool hold, const unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    switch (cmd)
    {
    case CMDS::ALL_ON:
    case CMDS::ALL_OFF:
    case CMDS::CH1_ON:
    case CMDS::CH1_OFF:
    case CMDS::CH2_ON:
    case CMDS::CH2_OFF:
    case CMDS::CH3_ON:
    case CMDS::CH3_OFF:
    case CMDS::CH4_ON:
    case CMDS::CH4_OFF:
    case CMDS::DISCO_SPEED_PLUS:
    case CMDS::DISCO_SPEED_MINUS:
    case CMDS::DISCO:
        frame_[static_cast<uint8_t>(BYTE_POS::CMD)] = (hold) ? static_cast<uint8_t>(HOLD::HOLD) : 0x00;
    case ::CMDS::SET_LIGHT:
    case ::CMDS::SET_COLOR:
        frame_[static_cast<uint8_t>(BYTE_POS::CMD)] += static_cast<uint8_t>(cmd);
        break;
    default:
        return;
    }

    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendFromBuf(const unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    for (unsigned int i = 0; i < seqLength; ++i)
    {
        /*//print frame
        for (int k = 0; k < 7; ++k)
        {
            printf("%02X ", frame_[k]);
        }
        printf("\n");*/

        //copy frame to radio buffer and send
        radio_.write(frame_, 7);

        if (delayVal) delay(delayVal);

        //resend frame
        for (unsigned int j = 0; j < resends; ++j)
        {
            radio_.resend();
            if (delayVal) delay(delayVal);
        }

        //increment seq
        ++frame_[6];
    }
}

void MiLightPilot::SendFrame(vector<uint8_t> &frame, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    if (frame.size() == 7 && seqLength > 0)
    {
        memcpy(frame_, frame.data(), 7);
        SendFromBuf(resends, seqLength, delayVal);
    }
}

void MiLightPilot::SendALL_ON(bool hold, unsigned int resends , unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::ALL_ON, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendALL_OFF(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::ALL_OFF, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH1_ON(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH1_ON, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH1_OFF(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH1_OFF, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH2_ON(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH2_ON, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH2_OFF(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH2_OFF, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH3_ON(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH3_ON, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH3_OFF(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH3_OFF, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH4_ON(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH4_ON, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendCH4_OFF(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::CH4_OFF, hold);
    SendFromBuf(resends, seqLength, delayVal);
}
void MiLightPilot::SendDISCO_SPEED_PLUS(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::DISCO_SPEED_PLUS, hold);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendDISCO_SPEED_MINUS(bool hold, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetCmd(CMDS::DISCO_SPEED_MINUS, hold);
    SendFromBuf(resends, seqLength, delayVal);
}
void device::milight::MiLightPilot::SendDISCO(DISCO_MODES mode, bool hold , unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
	SetDisco(mode);
	SetCmd(CMDS::DISCO, hold);
	SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendSET_LIGHT(uint8_t light, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetLight(ClosestLightVal(light));
    SetCmd(CMDS::SET_LIGHT, false);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendSET_LIGHT(LIGHT_VALS light, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
	SetLight(light);
	SetCmd(CMDS::SET_LIGHT, false);
	SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendSET_COLOR(uint8_t color,const unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
    SetColor(color);
    SetCmd(CMDS::SET_COLOR, false);
    SendFromBuf(resends, seqLength, delayVal);
}

void MiLightPilot::SendSET_COLOR(COLORS color, unsigned int resends, unsigned int seqLength, unsigned int delayVal)
{
	SetColor(color);
}

uint8_t MiLightPilot::GetLightVal(unsigned int lightInPerc)
{
    if (lightInPerc >= 100)
        return static_cast<uint8_t>(LIGHT_VALS::LIGHT_MAX);
    else if(lightInPerc == 0)
        return static_cast<uint8_t>(LIGHT_VALS::LIGHT_MIN);

    uint8_t t = static_cast<uint8_t>(round(lightInPerc / 4.0f));
    uint8_t v = 0xFF - 0x7E - t * 8;
    if(v < 0)
        v = 0xFF + 0x82 - t * 8;
    return v;
}

uint8_t MiLightPilot::ClosestLightVal(uint8_t val)
{
	if ((val >= 0x0 && val <= 0x81) || (val >= 0xB9 && val <= 0xFF))
	{
		//hight byte
		uint8_t temp1 = val >> 4;
		temp1 = temp1 << 4;
		//low byte
		uint8_t temp2 = val << 4;
		temp2 = temp2 >> 4;
		temp2 = (temp2 > 5) ? 0x09 : 0x01;
		return temp1 | temp2;
	}
	else if (val <= 0x9D)
	{
		return static_cast<uint8_t>(0x81);
	}
	else
	{
		return static_cast<uint8_t>(0xB9);
	}
}

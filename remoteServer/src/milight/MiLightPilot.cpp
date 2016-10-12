#include <cstdio>

#include "MilightPilot.h"

using namespace device;
using namespace milight;
using namespace std;

MiLightPilot::MiLightPilot()
    :	channelID_(CH_ID::ALL),
        holdButton_(false),
        resends_(defaultResends),
        sequenceLength_(defaultSequenceLength),
        RF24_(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_1MHZ),
		PL1167_nRF24_(RF24_),
		radio_(PL1167_nRF24_)
{

}

MiLightPilot::~MiLightPilot()
{

}

void MiLightPilot::Init()
{
    radio_.begin();
}

void MiLightPilot::SendCmd(CMDS command)
{
    switch (command)
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
        frame_[BYTE_POS::CMD] = (holdButton_) ? HOLD : 0x00;
    case ::CMDS::SET_LIGHT://int this two cases remote seems not to send
    case ::CMDS::SET_COLOR://the hold bit
        frame_[BYTE_POS::CMD] += byte_cast(command);//in all valid cases set command
        break;
    default:
        return;
    }

    SendFromBuf();
}

void MiLightPilot::SendFromBuf()
{
    for (unsigned int i = 0; i < sequenceLength_; ++i)
    {
        //print frame
        #ifdef PILOT_DEBUG
        for (int k = 0; k < FRAME_LENGTH; ++k)
        {
            printf("%02X ", frame_[k]);
        }
        printf("\n");
        #endif // PILOT_DEBUG

        //copy frame to radio buffer and send
        radio_.write(frame_.data(), FRAME_LENGTH);
        //resend frame
        for (unsigned int j = 0; j < resends_; ++j)
        {
            radio_.resend();
        }
        //increment seq
        ++frame_[BYTE_POS::SEQ];
    }
}

void MiLightPilot::SendFrame(const vector<uint8_t>& frame)
{
    if (sequenceLength_ != 0)
    {
        SetFrame(frame);
        SendFromBuf();
    }
}

void MiLightPilot::SendALL_ON()
{
    SetCmd(CMDS::ALL_ON);
    SetChannelID(CH_ID::ALL);
    SendFromBuf();
}

void MiLightPilot::SendALL_OFF()
{
    SetCmd(CMDS::ALL_OFF);
    SetChannelID(CH_ID::ALL);
    SendFromBuf();
}

void MiLightPilot::SendCH1_ON()
{
    SetCmd(CMDS::CH1_ON);
    SetChannelID(CH_ID::CH1);
    SendFromBuf();
}

void MiLightPilot::SendCH1_OFF()
{
    SetCmd(CMDS::CH1_OFF);
    SetChannelID(CH_ID::CH1);
    SendFromBuf();
}

void MiLightPilot::SendCH2_ON()
{
    SetCmd(CMDS::CH2_ON);
    SetChannelID(CH_ID::CH2);
    SendFromBuf();
}

void MiLightPilot::SendCH2_OFF()
{
    SetCmd(CMDS::CH2_OFF);
    SetChannelID(CH_ID::CH2);
    SendFromBuf();
}

void MiLightPilot::SendCH3_ON()
{
    SetCmd(CMDS::CH3_ON);
    SetChannelID(CH_ID::CH3);
    SendFromBuf();
}

void MiLightPilot::SendCH3_OFF()
{
    SetCmd(CMDS::CH3_OFF);
    SetChannelID(CH_ID::CH3);
    SendFromBuf();
}

void MiLightPilot::SendCH4_ON()
{
    SetCmd(CMDS::CH4_ON);
    SetChannelID(CH_ID::CH4);
    SendFromBuf();
}

void MiLightPilot::SendCH4_OFF()
{
    SetCmd(CMDS::CH4_OFF);
    SetChannelID(CH_ID::CH4);
    SendFromBuf();
}
void MiLightPilot::SendDISCO_SPEED_PLUS()
{
    SetCmd(CMDS::DISCO_SPEED_PLUS);
    SendFromBuf();
}

void MiLightPilot::SendDISCO_SPEED_MINUS()
{
    SetCmd(CMDS::DISCO_SPEED_MINUS);
    SendFromBuf();
}
void device::milight::MiLightPilot::SendDISCO(DISCO_MODES mode)
{
	SetDisco(mode);
	SetCmd(CMDS::DISCO);
	SendFromBuf();
}

void MiLightPilot::SendSET_LIGHT(light_val light)
{
    bool hold = holdButton_;
    SetHoldButton(false);
    SetLight(light);
    SetCmd(CMDS::SET_LIGHT);
    SendFromBuf();
    SetHoldButton(hold);
}

void MiLightPilot::SendSET_COLOR(uint8_t color)
{
    bool hold = holdButton_;
    SetHoldButton(false);
    SetColor(color);
    SetCmd(CMDS::SET_COLOR);
    SendFromBuf();
    SetHoldButton(hold);
}

void MiLightPilot::SendSET_COLOR(COLORS color)
{
	SendSET_COLOR(byte_cast(color));
}


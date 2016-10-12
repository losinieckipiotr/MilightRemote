#ifndef MILIGHT_PILOT_H
#define MILIGHT_PILOT_H

#include <array>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

#include "MiLightRadio.h"
#include "PL1167_nRF24.h"

namespace device
{
namespace milight
{
enum class CMDS
{
    ALL_ON = 0x01,
    ALL_OFF = 0x02,
    CH1_ON = 0x03,
    CH1_OFF = 0x04,
    CH2_ON = 0x05,
    CH2_OFF = 0x06,
    CH3_ON = 0x07,
    CH3_OFF = 0x08,
    CH4_ON = 0x09,
    CH4_OFF = 0x0A,
    DISCO_SPEED_PLUS = 0x0B,
    DISCO_SPEED_MINUS = 0x0C,
    DISCO = 0x0D,
    SET_LIGHT = 0x0E,
    SET_COLOR = 0x0F,
};

enum class COLORS
{
    RED = 0x1E,
    BLUE = 0xB9,
    GREEN = 0x69,
    YELLOW = 0x40
};

enum class CH_ID
{
    ALL = 0x00,
    CH1 = 0x01,
    CH2 = 0x02,
    CH3 = 0x03,
    CH4 = 0x04
};

enum DISCO_MODES
{
    MODE0 = 0xB0,
    MODE1 = 0xB1,
    MODE2 = 0xB2,
    MODE3 = 0xB3,
    MODE4 = 0xB4,
    MODE5 = 0xB5,
    MODE6 = 0xB6,
    MODE7 = 0xB7,
    MODE8 = 0xB8,
    MODE9 = 0xB9
};

enum BYTE_POS
{
    DISCO = 0,
    ID_1 = 1,
    ID_2 = 2,
    COLOR = 3,
    LIGHT_AND_CH = 4,
    CMD = 5,
    SEQ = 6
};

enum
{
    defaultResends = 16,
    defaultSequenceLength = 1
};

template<class T>
constexpr uint8_t byte_cast(T&& val)
{
    return static_cast<uint8_t>(std::forward<T>(val));
}

class light_val
{
public:
    light_val(unsigned int lightInPerc) : val(GetLightVal(lightInPerc)) { }

    uint8_t GetVal() const noexcept { return this->operator uint8_t(); }

    operator uint8_t() const noexcept { return val; }

    static constexpr auto MIN = 0x80;
    static constexpr auto MAX = 0xB8;

    private:
    static uint8_t GetLightVal(unsigned int lightInPerc)
    {
        if (lightInPerc == 0)
            return byte_cast(MIN);
        else if(lightInPerc >= 100)
            return byte_cast(MAX);

        int t = round(lightInPerc / 4.0f);
        int v = 0xFF - 0x7F - t * 8;
        if(v < 0)
            v = 0xFF + 0x81 - t * 8;
        return byte_cast(v);
    }

    uint8_t val;
};

struct send_params
{
    unsigned int resends = defaultResends;
    unsigned int sequenceLength = defaultSequenceLength;
};

constexpr auto HOLD = 0x10;
constexpr auto ID_LENGTH = 2;
constexpr auto FRAME_LENGTH = 7;

class MiLightPilot
{
public:
    MiLightPilot();
    ~MiLightPilot();

    MiLightPilot(const MiLightPilot&) = delete;
    MiLightPilot(MiLightPilot&&) = delete;

    MiLightPilot& operator=(const MiLightPilot&) = delete;
    MiLightPilot& operator=(MiLightPilot&&) = delete;

    void Init();

    inline void SetID(const std::pair<uint8_t, uint8_t >& id) noexcept
    {
        frame_[ID_1] = std::get<0>(id);
        frame_[ID_2] = std::get<1>(id);
    }
    inline void SetDisco(DISCO_MODES mode) noexcept
    {
        frame_[BYTE_POS::DISCO] = byte_cast(mode);
    }
    inline void SetColor(uint8_t color) noexcept
    {
        frame_[BYTE_POS::COLOR] = color;
    }
    inline void SetColor(COLORS color) noexcept
    {
        frame_[BYTE_POS::COLOR] = byte_cast(color);
    }
    inline void SetChannelID(CH_ID channelID) noexcept
    {
        channelID_ = channelID;
    }
    inline void SetLight(light_val light) noexcept
    {
        frame_[BYTE_POS::LIGHT_AND_CH] =
            light.GetVal() + byte_cast(channelID_);
    }
    inline void SetLightInPerc(unsigned int light)
    {
        frame_[BYTE_POS::LIGHT_AND_CH] =
            light_val(light).GetVal() + byte_cast(channelID_);
    }
    inline void SetHoldButton(bool holdButton) noexcept
    {
        holdButton_ = holdButton;
    }
    inline void SetCmd(CMDS cmd)
    {
        frame_[BYTE_POS::CMD] = (holdButton_) ?
            byte_cast(cmd) + HOLD : byte_cast(cmd);
    }
    inline void SetSeq(uint8_t seq)
    {
        frame_[BYTE_POS::SEQ] = seq;
    }

    inline void SetSendParams(const send_params& params) noexcept
    {
        resends_ = params.resends;
        sequenceLength_ = params.sequenceLength;
    }

    inline std::vector<uint8_t> GetFrame() const
    {
        return std::vector<uint8_t>(frame_.cbegin(), frame_.cend());
    }

    inline void SetFrame(const std::vector<uint8_t>& frame)
    {
        if (frame.size() >= FRAME_LENGTH)
        {
            std::copy(frame.cbegin(), frame.cend(), frame_.begin());
        }
    }

    void SendCmd(CMDS command);
    void SendFromBuf();
    void SendFrame(const std::vector<uint8_t>& frame);

    void SendALL_ON();
    void SendALL_OFF();
    void SendCH1_ON();
    void SendCH1_OFF();
    void SendCH2_ON();
    void SendCH2_OFF();
    void SendCH3_ON();
    void SendCH3_OFF();
    void SendCH4_ON();
    void SendCH4_OFF();
    void SendDISCO_SPEED_PLUS();
    void SendDISCO_SPEED_MINUS();
    void SendDISCO(DISCO_MODES mode);
    void SendSET_LIGHT(light_val light);
    void SendSET_COLOR(uint8_t color);
    void SendSET_COLOR(COLORS color);

private:
    CH_ID channelID_;
    bool holdButton_;

    unsigned int resends_;
    unsigned int sequenceLength_;

    std::array<uint8_t, FRAME_LENGTH> frame_;

    RF24 RF24_;
    PL1167_nRF24 PL1167_nRF24_;
    MiLightRadio radio_;
};
}
}

#endif // MILIGHT_PILOT_H

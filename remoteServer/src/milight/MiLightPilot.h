#ifndef MI_LIGHT_PILOT_H
#define MI_LIGHT_PILOT_H

#include <vector>
#include <cstring>

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

		enum class LIGHT_VALS
		{
			LIGHT_MIN = 0x81,
			LIGHT_MAX = 0xB9
		};

		enum DISCO_MODES
		{
			MODE0 = 0xB0,
			MODE1 = 0xB1,
			MODE2 = 0xB3,
			MODE3 = 0xB4
		};

		enum class HOLD
		{
			HOLD = 0x10
		};

		enum class BYTE_POS
		{
			ID = 0,
			DISCO = 0,
			COLOR = 3,
			LIGHT = 4,
			CMD = 5,
			SEQ = 6
		};

		enum
		{
			defaultResends = 0,
			defaultSeqLength = 16,
			defaultDelay = 0
		};

		class MiLightPilot
		{
		public:
			MiLightPilot();
			virtual ~MiLightPilot();

			void Init();

			inline void SetID(std::vector<uint8_t> id)
			{
				if (id.size() == 3)
				memcpy(frame_ + static_cast<size_t>(BYTE_POS::ID), id.data(), 3);
			}
			inline void SetDisco(DISCO_MODES mode)
			{
				frame_[static_cast<size_t>(BYTE_POS::DISCO)] = static_cast<uint8_t>(mode);
			}
			inline void SetColor(uint8_t color)
			{
				frame_[static_cast<size_t>(BYTE_POS::COLOR)] = color;
			}
			inline void SetColor(COLORS color)
			{
				frame_[static_cast<size_t>(BYTE_POS::COLOR)] = static_cast<uint8_t>(color);
			}
			inline void SetLight(uint8_t light)
			{
				frame_[static_cast<size_t>(BYTE_POS::LIGHT)] = light;
			}
			inline void SetLight(LIGHT_VALS light)
			{
				frame_[static_cast<size_t>(BYTE_POS::LIGHT)] = static_cast<uint8_t>(light);
			}
			inline void SetLightInPerc(unsigned int light)
			{
				frame_[static_cast<size_t>(BYTE_POS::LIGHT)] = GetLightVal(light);
			}
			inline void SetCmd(uint8_t cmd, bool hold = false)
			{
				frame_[static_cast<size_t>(BYTE_POS::CMD)] = (hold) ?
					static_cast<uint8_t>(HOLD::HOLD) + cmd : cmd;
			}
			inline void SetCmd(CMDS cmd, bool hold = false)
			{
				frame_[static_cast<size_t>(BYTE_POS::CMD)] = (hold) ?
					static_cast<uint8_t>(static_cast<uint8_t>(HOLD::HOLD) + static_cast<uint8_t>(cmd)) :
					static_cast<uint8_t>(cmd);
			}
			inline void SetSeq(uint8_t seq)
			{
				frame_[static_cast<size_t>(BYTE_POS::SEQ)] = seq;
			}

			void SendCmd(CMDS,bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendFromBuf(unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendFrame(std::vector<uint8_t> &, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);

			void SendALL_ON(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendALL_OFF(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH1_ON(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH1_OFF(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH2_ON(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH2_OFF(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH3_ON(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH3_OFF(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH4_ON(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendCH4_OFF(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendDISCO_SPEED_PLUS(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendDISCO_SPEED_MINUS(bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendDISCO(DISCO_MODES, bool = false, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);

			void SendSET_LIGHT(uint8_t, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendSET_LIGHT(LIGHT_VALS, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendSET_COLOR(uint8_t, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);
			void SendSET_COLOR(COLORS, unsigned int = defaultResends, unsigned int = defaultSeqLength, unsigned int = defaultDelay);

			uint8_t GetLightVal(unsigned int);
			uint8_t ClosestLightVal(uint8_t);

		protected:
			uint8_t frame_[7];

		private:
			RF24 RF24_;
			PL1167_nRF24 PL1167_nRF24_;
			MiLightRadio radio_;
		};
	}
}

#endif // MI_LIGHT_PILOT_H

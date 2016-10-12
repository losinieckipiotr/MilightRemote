#ifndef RECIVEHANDLER_H
#define RECIVEHANDLER_H
#include <cstdint>
#include <string>
#include <deque>
#include <vector>
#include <mutex>

#include "milight/MilightPilot.h"

class MiLightCommand
{
    public:
        MiLightCommand(
            std::vector<uint8_t>&& bytes,
            unsigned int resends,
            unsigned int sequenceLength)
            :   bytes(std::move(bytes)),
                resends(resends),
                sequenceLength(sequenceLength) { }

    std::vector<uint8_t> bytes;
    unsigned int resends;
    unsigned int sequenceLength;
};

class ReciveHandler
{
    using jsonList = std::deque<std::string>;
    using milightCommands = std::deque<MiLightCommand>;

    public:
        ReciveHandler();

        void Handle(std::string&& recivedBytes);

    private:
        void SeekJSONs();
        void DecodeJSONs();
        void SendFrames();

        std::mutex handleMtx_;

        std::string buffer_;
        jsonList jsons_;
        milightCommands commands_;
        device::milight::MiLightPilot pilot_;
};

#endif // RECIVEHANDLER_H

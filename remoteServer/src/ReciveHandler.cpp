#include <sstream>
#include <future>
#include <cmath>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "ReciveHandler.h"

using namespace std;
namespace pt = boost::property_tree;

ReciveHandler::ReciveHandler()
{
    pilot_.Init();
}

void ReciveHandler::Handle(string&& recivedBytes)
{
    const auto handler = [this](string&& newBytes)->void
    {
        try
        {
            lock_guard<mutex> lck(handleMtx_);

            buffer_.append(newBytes);
            SeekJSONs();
            DecodeJSONs();
            SendFrames();
        }
        catch (exception& e)
        {
            cout << "ReciveHandler::Handle exception:" << endl;
            cout << e.what() << endl;
        }
    };
    async(launch::async, handler, move(recivedBytes));
}

void ReciveHandler::SeekJSONs()
{
    cout << "SeekJSONs(), buffer_.size(): " << buffer_.size() << endl;

    while(!buffer_.empty())
    {
        auto jsonStart = buffer_.find_first_of('{');
        auto jsonEnd = buffer_.find_first_of('}', jsonStart);
        if(jsonStart == string::npos)
        {
            cout << "Begin of JSON has not been found" << endl;
            cout << buffer_ << endl;
            buffer_.clear();
        }
        else if(jsonEnd == string::npos)
        {
            //cout << "End of JSON has not been found" << endl;
            //cout << buffer_ << endl;
            break;
        }
        else
        {
            jsons_.push_back(
                std::move(buffer_.substr(jsonStart, jsonEnd-jsonStart + 1)));
            buffer_ = buffer_.substr(jsonEnd + 1);
        }
    }
}

void ReciveHandler::DecodeJSONs()
{
    cout << "DecodeJSONs(), jsons_.size(): " << jsons_.size() << endl;

    while(!jsons_.empty())
    {
        stringstream jsonStr(jsons_.front());
        jsons_.pop_front();

        pt::ptree tree;
        pt::read_json(jsonStr, tree);

        vector<string> bytesStr;
        bytesStr.reserve(7);
        for(const auto& byte : tree.get_child("frame"))
        {
            bytesStr.push_back(byte.second.data());
        }

        vector<uint8_t> bytes;
        bytes.reserve(7);
        for(const auto& byte : bytesStr)
        {
            auto num = stoi(byte, nullptr, 16);
            bytes.push_back(static_cast<uint8_t>(num));
        }

        auto resends = tree.get<unsigned int>("resendsNumber");
        auto sequence = tree.get<unsigned int>("sequenceLength");

        commands_.emplace_back(
            std::move(bytes),
            resends,
            sequence);
    }
}

void ReciveHandler::SendFrames()
{
    cout << "SendFrames(), commands_.size: " << commands_.size() << endl;

    //unsigned int i = 0;
    for(auto& cmd : commands_)
    {
        pilot_.SendFrame(cmd.bytes, cmd.resends, cmd.seqLength, 0);

        //cout << "cmd send: " << i++ << endl;
    }
    commands_.clear();
    cout << "DONE" << endl;
}

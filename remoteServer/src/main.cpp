#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <vector>

#include "TcpServer.h"
#include "milight/MiLightPilot.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
namespace pt = boost::property_tree;

int main()
{
    cout << "remoteServer" << endl;

    mutex handlerMtx;
    device::milight::MiLightPilot pilot;
    pilot.Init();

    auto frameHander = [&handlerMtx, &pilot](const string&& frame)
    {
        try
        {
            lock_guard<mutex> lck(handlerMtx);

            stringstream jsonStr(frame);
            pt::ptree tree;
            pt::read_json(jsonStr, tree);

            vector<string> bytesStr;
            bytesStr.reserve(7);

            //string bytes = "";
            for(const auto& byte : tree.get_child("frame"))
            {
                //bytes += byte.second.data() + " ";
                bytesStr.push_back(byte.second.data());
            }
            //stringstream hexCovertStream;
            vector<uint8_t> bytes;
            bytes.reserve(7);

            for(const auto& byte : bytesStr)
            {
                auto num = stoi(byte, nullptr, 16);
                bytes.push_back(static_cast<uint8_t>(num));
            }

            auto resends = tree.get<int>("resendsNumber");
            auto sequence = tree.get<int>("sequenceLength");

            //pilot.SendFrame(bytes, resends, sequence, 0);

            /*cout << "frame:" << endl;
            for(const auto& byte : bytesStr)
            {
                cout << "0x" << byte <<  " ";
            }
            cout << endl;
            cout << "resendsNumber: " << resends << endl;
            cout << "sequenceLength: " << sequence << endl;*/
        }
        catch (exception& e)
        {
            cout << "frameHander exception:" << endl;
            cout << e.what() << endl;
            cout << frame << endl;
        }
    };

    const uint16_t port = 2016;
    TcpServer server(port, frameHander);

    cout << "Starting server on port: " << port << "..." <<endl;

    server.Start();

    cin.get();
    cout << "Shutting down server" << endl;

    server.Shutdown();

    return 0;
}

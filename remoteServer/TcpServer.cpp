#include "TcpServer.h"
#include <iostream>
#include <exception>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

namespace pt = boost::property_tree;

TcpServer::TcpServer(const uint16_t port) :
    service_(),
    socket_(service_),
    acceptor_(service_,
    tcp::endpoint(tcp::v4(), (unsigned short)port), true)
{

}

TcpServer::~TcpServer()
{

}

void TcpServer::Start()
{
    Accept();
    serverTh_ = thread([this](){ service_.run(); });
}

void TcpServer::Accept()
{
    cout << "TcpServer::Accept()" << endl;
    socket_ = tcp::socket(service_);
    acceptor_.async_accept(socket_,
    [this](const boost::system::error_code& ec)
    {
        if(!ec)
        {
            tcp::no_delay option(true);
            socket_.set_option(option);
            cout << "Connection accepted." << endl;
            Recive();
        }
        else
        {
            cout << "Accept error: " << ec.message() << endl;
        }
    });
}

void TcpServer::Recive()
{
    cout << "TcpServer::Recive()" << endl;
    socket_.async_read_some(
        buffer(data_.begin(), data_.size()),
        [this](const boost::system::error_code& ec, std::size_t length)
        {
            if (!ec)
            {
                Recive();
                frame_ = string(data_.cbegin(), length);

                try
                {
                    stringstream ss(frame_);
                    pt::ptree tree;
                    pt::read_json(ss, tree);

                    string bytes = "";
                    for(auto& byte : tree.get_child("frame"))
                    {
                        bytes += byte.second.data() + " ";
                    }

                    int resends = tree.get<int>("resendsNumber");
                    int sequence = tree.get<int>("sequenceLength");

                    cout << bytes << endl;
                    cout << "resendsNumber: " << resends << endl;
                    cout << "sequenceLength: " << sequence << endl;
                }
                catch (exception& e)
                {
                    cout << e.what() << endl;
                }
            }
            else
            {
                cout << "Recive error: " << ec.message() << endl;
                Accept();
            }
        });
}

void TcpServer::Shutdown()
{
    socket_.close();
    service_.stop();
    serverTh_.join();
}

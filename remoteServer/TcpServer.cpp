#include "TcpServer.h"
#include <iostream>
#include <exception>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

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
    socket_ = tcp::socket(service_);
    acceptor_.async_accept(socket_,
    [this](const boost::system::error_code& ec)
    {
        if(!ec)
        {
            tcp::no_delay option(true);
            socket_.set_option(option);
            Recive();
        }
        else
        {
            cout << "Accept Error!" << endl;
        }
    });
}

void TcpServer::Recive()
{
    socket_.async_read_some(
        buffer(data_.begin(), data_.size()),
        [this](const boost::system::error_code& ec, std::size_t length)
        {
            if (!ec)
            {
                frame_ = string(data_.cbegin(), length);
                cout << frame_ << endl;
                Recive();
            }
        });
}

void TcpServer::Shutdown()
{
    socket_.close();
    service_.stop();
    serverTh_.join();
}

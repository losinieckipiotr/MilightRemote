#include <iostream>

#include "TcpServer.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

TcpServer::TcpServer(const uint16_t port,
                     ReciveHandler& handler) :
    handler_(handler),
    service_(),
    acceptor_(  service_,
                tcp::endpoint(tcp::v4(), port),
                true),
    socket_(service_)
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

void TcpServer::Shutdown()
{
    socket_.close();
    service_.stop();
    serverTh_.join();
}

void TcpServer::Accept()
{
    //cout << "TcpServer::Accept()" << endl;

    socket_ = tcp::socket(service_);
    acceptor_.async_accept(socket_,
    [this](const boost::system::error_code& ec)
    {
        if(!ec)
        {
            cout << "New connection" << endl;

            tcp::no_delay option(true);
            socket_.set_option(option);

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
    //cout << "TcpServer::Recive()" << endl;

    socket_.async_read_some(
        buffer(data_.begin(), data_.size()),
        [this](const boost::system::error_code& ec, std::size_t length)
        {
            if (!ec)
            {
                handler_.Handle(move(string(data_.cbegin(), length)));
                Recive();
            }
            else
            {
                cout << "Recive error: " << ec.message() << endl;

                Accept();
            }
        });
}

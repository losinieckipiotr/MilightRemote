#include "TcpServer.h"
#include <iostream>
#include <exception>
#include <future>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

TcpServer::TcpServer(const uint16_t port,
                     function<void(const string&&)> frameHandler) :
    frameHandler_(frameHandler),
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

            buffer_.clear();
            buffer_.reserve(buff_size);

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
                //cout << "Recived: " << length << "bytes" << endl;

                //TO DO: json queue

                buffer_.append(data_.cbegin(), length);
                auto jsonStart = buffer_.find_first_of('{');
                auto jsonEnd = buffer_.find_first_of('}', jsonStart);
                if(jsonStart == string::npos)
                {
                    cout << "Begin of JSON has not been found" << endl;
                    buffer_.clear();
                }
                else if(jsonEnd == string::npos)
                {
                    cout << "End of JSON has not been found" << endl;
                }
                else
                {
                    string json = buffer_.substr(jsonStart, jsonEnd-jsonStart + 1);
                    buffer_ = buffer_.substr(jsonEnd + 1);
                    if(!buffer_.empty())
                    {
                        cout << "Buffer left:" << endl;
                        cout << buffer_ << endl;
                    }
                    async(
                    launch::async,
                    frameHandler_,
                    move(json));
                }

                Recive();
            }
            else
            {
                cout << "Recive error: " << ec.message() << endl;

                Accept();
            }
        });
}

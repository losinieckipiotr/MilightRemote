#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstdint>
#include <array>
#include <thread>

#include <boost/asio.hpp>

#include "ReciveHandler.h"

class TcpServer
{
public:
	TcpServer(const std::uint16_t port,
            ReciveHandler& handler);
	~TcpServer();

	void Start();
	void Shutdown();

private:
    void Accept();
    void Recive();

    ReciveHandler& handler_;

    boost::asio::io_service service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    std::thread serverTh_;

    enum { buff_size = 1024*1024 };
    std::array<char, buff_size> data_;
};

#endif // !TCP_SERVER_h

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstdint>
#include <string>
#include <array>
#include <thread>

#include <boost/asio.hpp>

class TcpServer
{
public:
	TcpServer(const std::uint16_t port);
	~TcpServer();

	void Start();
	void Shutdown();

private:
    void Accept();
    void Recive();

    boost::asio::io_service service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::acceptor acceptor_;

    std::thread serverTh_;

    enum { buff_size = 1024 };
    std::array<char, buff_size> data_;
    std::string frame_;
};

#endif // !TCP_SERVER_h

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstdint>
#include <functional>
#include <string>
#include <array>
#include <thread>
//#include <queue>

#include <boost/asio.hpp>

class TcpServer
{
public:
	TcpServer(const std::uint16_t port,
              std::function<void(const std::string&&)> frameHandler);
	~TcpServer();

	void Start();
	void Shutdown();

private:
    void Accept();
    void Recive();

   //void HandleFrame(const std::string& frame);
    std::function<void(const std::string&&)> frameHandler_;

    boost::asio::io_service service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    std::thread serverTh_;

    enum { buff_size = 1024 };
    std::array<char, buff_size> data_;
    std::string buffer_;
    //std::queue<char> fifo_;
};

#endif // !TCP_SERVER_h

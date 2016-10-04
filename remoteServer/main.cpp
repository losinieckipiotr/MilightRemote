#include <iostream>
#include "TcpServer.h"

using namespace std;

int main()
{
    cout << "remoteServer" << endl;

    const uint16_t port = 2016;
    TcpServer server(port);

    cout << "Starting server on port: " << port << "...";

    server.Start();

    cin.get();
    cout << "Shutting down server" << endl;

    server.Shutdown();

    return 0;
}

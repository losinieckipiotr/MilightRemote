#include <iostream>

#include "milight/MiLightPilot.h"
#include "ReciveHandler.h"
#include "TcpServer.h"

using namespace std;

int main()
{
    cout << "remoteServer" << endl;

    ReciveHandler handler;
    const uint16_t port = 2016;
    TcpServer server(port, handler);

    cout << "Starting server on port: " << port << "..." <<endl;

    server.Start();

    cin.get();
    cout << "Shutting down server" << endl;

    server.Shutdown();

    return 0;
}

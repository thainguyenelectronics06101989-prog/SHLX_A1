#include "ClientHandler.h"

ClientHandler::ClientHandler()
{
    serverIP.fromString(DEFAUT_SERVER_IP);
}
ClientHandler::~ClientHandler()
{
}
void ClientHandler::setHardwareAdapter(HardwareManager &hw)
{
    hardwareManager = &hw;
    hardwareConfiged = true;
}
bool ClientHandler::configServer(String ip, String p)
{
    port = p.toInt();
    return serverIP.fromString(ip);
}
bool ClientHandler::startClient()
{
    return client.connect(serverIP, port);
}
bool ClientHandler::startClient(String ip, String p)
{
    configServer(ip, p);
    return client.connect(serverIP, port);
}
void ClientHandler::checkClient()
{
    if (client.available() > 0)
    {
        uint8_t c = client.read();
    }
    // client.stop();
}
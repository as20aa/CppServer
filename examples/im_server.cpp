/*!
    \file im_server.cpp
    \brief IM WebSocket chat server example
    \author as
    \date 21.03.2023
    \copyright MIT License
*/

#include "asio_service.h"

#include "server/ws/ws_server.h"

#include "system/uuid.h"

#include <iostream>
#include <map>

class ChatSession : public CppServer::WS::WSSession
{
public:
    using CppServer::WS::WSSession::WSSession;
private:
    

protected:
    // when use the websocket connect to the server, the onWSConnected is called
    void onWSConnected(const CppServer::HTTP::HTTPRequest& request) override
    {
        std::cout << "Chat WebSocket session with Id " << id() << " connected!" << std::endl;

        // the connection id will add to the server,so you would not add it by yourself.
        // Send invite message
        //std::string message("Hello from WebSocket chat! Please send a message or '!' to disconnect the client!");
        // this will sendback the message to the client
        //SendTextAsync(message);
    }

    void onWSDisconnected() override
    {
        std::cout << "Chat WebSocket session with Id " << id() << " disconnected!" << std::endl;
    }


    // receive message from clients
    void onWSReceived(const void* buffer, size_t size) override
    {
        std::string message((const char*)buffer, size);
        std::cout << "Incoming: " << message << " from " << id() << std::endl;

        // Multicast message to all connected sessions
        // std::dynamic_pointer_cast<CppServer::WS::WSServer>(server())->MulticastText(message);
        // construct the UUID from message
        try{
            CppCommon::UUID uuid = CppCommon::UUID(message.substr(0, 36));
            // get the server
            std::shared_ptr<CppServer::Asio::TCPSession> session = std::dynamic_pointer_cast<CppServer::WS::WSServer>(server())->FindSession(uuid);

            // transfer to the target session
            std::dynamic_pointer_cast<CppServer::WS::WSSession>(session)->SendTextAsync(buffer, size);
            std::cout << "resend the message" << std::endl;
        } catch(int e) {
            std::cout << "error during resending message!" << std::endl;
        }
         // If the buffer starts with '!' the disconnect the current session
        if (message == "!")
            Close(1000);
    }

    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "Chat WebSocket session caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};

class ChatServer : public CppServer::WS::WSServer
{
public:
    using CppServer::WS::WSServer::WSServer;

protected:
    std::shared_ptr<CppServer::Asio::TCPSession> CreateSession(const std::shared_ptr<CppServer::Asio::TCPServer>& server) override
    {
        return std::make_shared<ChatSession>(std::dynamic_pointer_cast<CppServer::WS::WSServer>(server));
    }

protected:
    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "Chat WebSocket server caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};

int main(int argc, char** argv)
{
    // WebSocket server port
    int port = 8080;
    if (argc > 1)
        port = std::atoi(argv[1]);
    // WebSocket server content path
    std::string www = "../www/ws";
    if (argc > 2)
        www = argv[2];

    std::cout << "WebSocket server port: " << port << std::endl;
    std::cout << "WebSocket server static content path: " << www << std::endl;
    std::cout << "WebSocket server website: " << "http://localhost:" << port << "/chat/index.html" << std::endl;

    std::cout << std::endl;

    // Create a new Asio service
    auto service = std::make_shared<AsioService>();

    // Start the Asio service
    std::cout << "Asio service starting...";
    service->Start();
    std::cout << "Done!" << std::endl;

    // Create a new WebSocket chat server
    auto server = std::make_shared<ChatServer>(service, port);
    server->AddStaticContent(www, "/chat");

    // Start the server
    std::cout << "Server starting...";
    server->Start();
    std::cout << "Done!" << std::endl;

    std::cout << "Press Enter to stop the server or '!' to restart the server..." << std::endl;

    // Perform text input
    std::string line;
    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        // Restart the server
        if (line == "!")
        {
            std::cout << "Server restarting...";
            server->Restart();
            std::cout << "Done!" << std::endl;
            continue;
        }

        // Multicast admin message to all sessions
        // line = "(admin) " + line;
        // server->MulticastText(line);
    }

    // Stop the server
    std::cout << "Server stopping...";
    server->Stop();
    std::cout << "Done!" << std::endl;

    // Stop the Asio service
    std::cout << "Asio service stopping...";
    service->Stop();
    std::cout << "Done!" << std::endl;

    return 0;
}

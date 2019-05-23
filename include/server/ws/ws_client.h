/*!
    \file ws_client.h
    \brief WebSocket client definition
    \author Ivan Shynkarenka
    \date 22.05.2019
    \copyright MIT License
*/

#ifndef CPPSERVER_HTTP_WS_CLIENT_H
#define CPPSERVER_HTTP_WS_CLIENT_H

#include "server/http/http_client.h"

namespace CppServer {
namespace WS {

//! WebSocket client
/*!
    WebSocket client is used to communicate with WebSocket server.

    Thread-safe.
*/
class WSClient : public HTTP::HTTPClient
{
public:
    using HTTPClient::HTTPClient;

    WSClient(const WSClient&) = delete;
    WSClient(WSClient&&) = delete;
    virtual ~WSClient() = default;

    WSClient& operator=(const WSClient&) = delete;
    WSClient& operator=(WSClient&&) = delete;

    bool Connect() override;
    bool Connect(std::shared_ptr<Asio::TCPResolver> resolver) override;
    bool ConnectAsync() override;
    bool ConnectAsync(std::shared_ptr<Asio::TCPResolver> resolver) override;

protected:
    void onConnected() override;
    void onDisconnected() override;
    void onReceived(const void* buffer, size_t size) override;
    void onReceivedResponseHeader(const HTTP::HTTPResponse& response) override;

    //! Handle WebSocket client connecting notification
    /*!
        Notification is called when WebSocket client is connecting
        to the server. You can handle the connection and change
        WebSocket upgrade HTTP request by providing your own headers.

        \param request - WebSocket upgrade HTTP request
    */
    virtual void onWSConnecting(HTTP::HTTPRequest& request) {}
    //! Handle WebSocket client connected notification
    virtual void onWSConnected(const HTTP::HTTPResponse& response) {}
    //! Handle WebSocket client disconnected notification
    virtual void onWSDisconnected() {}

private:
    // Handshaked flag
    bool _handshaked{false};
    // Sync connect flag
    bool _sync_connect;
};

/*! \example ws_client.cpp WebSocket client example */

} // namespace WS
} // namespace CppServer

#endif // CPPSERVER_HTTP_WS_CLIENT_H
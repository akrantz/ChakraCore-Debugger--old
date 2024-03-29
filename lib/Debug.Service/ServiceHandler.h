#pragma once

#include <ChakraDebugProtocolHandler.h>
#include <string>

namespace JsDebug
{
    class ServiceHandler
    {
    public:
        ServiceHandler(
            websocketpp::server<websocketpp::config::asio>* server,
            const char* id,
            JsDebugProtocolHandler protocolHandler,
            bool breakOnNextLine);
        ~ServiceHandler();

        bool RegisterConnection(websocketpp::connection_hdl hdl);

    private:
        static void CHAKRA_CALLBACK SendResponseCallback(const char* response, void* callbackState);
        void SendResponse(const char* response);

        void OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

        websocketpp::server<websocketpp::config::asio>* m_server;
        std::string m_id;
        JsDebugProtocolHandler m_protocolHandler;
        bool m_breakOnNextLine;

        websocketpp::connection_hdl m_hdl;
    };
}

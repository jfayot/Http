#ifndef HTTP_SESSION_SSL_HPP_INCLUDED
#define HTTP_SESSION_SSL_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/detail/HttpClientImpl.h"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/HttpSessionBase.hpp"

namespace Http::detail
{
    class SessionSsl : public SessionBase<SessionSsl>, public std::enable_shared_from_this<SessionSsl>
    {
    private:
        boost::asio::ssl::context m_ctx;
        boost::beast::ssl_stream<boost::beast::tcp_stream> m_stream;

    public:
        SessionSsl() = delete;
        SessionSsl(TClientImplPtr clientImpl, boost::asio::io_context& ioc, const std::string& host, const std::string& port, const Options& options)
            : SessionBase<SessionSsl>{ clientImpl, ioc, host, port, options }
            , m_ctx{ boost::asio::ssl::context::tlsv12_client }
            , m_stream{ boost::asio::make_strand(ioc), m_ctx }
        {
            if (true == options.ca().has_value())
            {
                m_ctx.load_verify_file(options.ca().value().string());
            }

            m_stream.set_verify_mode(true == options.ca().has_value() ? boost::asio::ssl::verify_peer : boost::asio::ssl::verify_none);
        }
        virtual ~SessionSsl() = default;
        SessionSsl(const SessionSsl &other) = delete;
        SessionSsl &operator=(const SessionSsl &other) = delete;
        SessionSsl(SessionSsl &&other) = delete;
        SessionSsl &operator=(SessionSsl &&other) = delete;

        [[nodiscard]]
        boost::beast::ssl_stream<boost::beast::tcp_stream>& stream() { return m_stream; }

        void onConnected()
        {
            m_stream.async_handshake(boost::asio::ssl::stream_base::client, boost::beast::bind_front_handler(&SessionSsl::onHandshaked, shared_from_this()));
        }

        void onHandshaked(const boost::beast::error_code& ec)
        {
            if (ec) return handleError(ec, "Socket handshake failed");

            if (m_cancel) return handleCancel();

            writeRequest();
        }
    };
}

#endif

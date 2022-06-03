#ifndef HTTP_SESSIO_PLAIN_HPP_INCLUDED
#define HTTP_SESSIO_PLAIN_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/detail/HttpClientImpl.h"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/HttpSessionBase.hpp"

namespace Http::detail
{
    class SessionPlain : public SessionBase<SessionPlain>, public std::enable_shared_from_this<SessionPlain>
    {
    private:
        boost::beast::tcp_stream m_stream;

    public:
        SessionPlain() = delete;
        SessionPlain(TClientImplPtr clientImpl, boost::asio::io_context& ioc, const std::string& host, const std::string& port, const Options& options)
            : SessionBase<SessionPlain>{ clientImpl, ioc, host, port, options }
            , m_stream{ boost::asio::make_strand(ioc) }
        {}

        virtual ~SessionPlain() = default;
        SessionPlain(const SessionPlain &other) = delete;
        SessionPlain &operator=(const SessionPlain &other) = delete;
        SessionPlain(SessionPlain &&other) = delete;
        SessionPlain &operator=(SessionPlain &&other) = delete;

        [[nodiscard]]
        boost::beast::tcp_stream& stream() { return m_stream; }

        void onConnected()
        {
            writeRequest();
        }
    };
}

#endif

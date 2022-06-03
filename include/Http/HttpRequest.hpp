#ifndef HTTP_REQUEST_HPP_INCLUDED
#define HTTP_REQUEST_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/HttpBody.hpp"
#include "Http/HttpResponse.hpp"
#include "Http/HttpMimeTypes.hpp"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/HttpClientImpl.h"

namespace Http
{
    class Request
    {
    private:
        friend struct detail::ClientImpl;

        detail::TRequestImplPtr m_requestImpl;
        detail::TClientImplPtr m_clientImpl;

    public:
        Request() = delete;
        virtual ~Request() = default;
        Request(const Request &other) = default;
        Request &operator=(const Request &other) = default;
        Request(Request &&other) = default;
        Request &operator=(Request &&other) = default;

        Request &header(const std::string &key, const std::string &value)
        {
            m_requestImpl->header(key, value);
            return *this;
        }

        Request &options(const Options &options)
        {
            m_requestImpl->options(options);
            return *this;
        }

        const Options &options() const
        {
            return m_requestImpl->m_options;
        }

        const Body &body()
        {
            return m_requestImpl->m_body;
        }

        template <typename ContentType>
        Request &body(ContentType &&content)
        {
            m_requestImpl->body(std::forward<ContentType>(content));
            return *this;
        }

        std::shared_future<Response> send() const
        {
            return m_clientImpl->send(m_requestImpl);
        }

        void cancel() const
        {
            m_requestImpl->cancel();
        }

        std::string dump() const
        {
            return m_requestImpl->dump();
        }

    private:
        Request(const std::string &host, const std::string &port, boost::beast::http::verb verb, const std::string &target, detail::TClientImplPtr clientImpl)
            : m_requestImpl{std::make_shared<detail::RequestImpl>(host, port, verb, target, clientImpl)}, m_clientImpl{clientImpl}
        {
            if (nullptr == m_requestImpl)
            {
                throw std::bad_alloc();
            }
        }
    };
}

#endif

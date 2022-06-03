#ifndef HTTP_SESSION_BASE_HPP_INCLUDED
#define HTTP_SESSION_BASE_HPP_INCLUDED

#include "Http/detail/HttpSessionBase.h"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/empty_body.hpp"

namespace Http::detail
{
    template <typename DerivedType>
    inline SessionBase<DerivedType>::SessionBase(TClientImplPtr clientImpl, boost::asio::io_context& ioc, const std::string& host, const std::string& port, const Options& options)
        : m_id{ boost::uuids::random_generator()() }
        , m_clientImpl{ clientImpl }
        , m_host{ host }
        , m_port{ port }
        , m_url{ host + ":" + port }
        , m_options{ options }
        , m_resolver{ boost::asio::make_strand(ioc) }
        , m_buffer{ Options::DefaultBufferSize }
        , m_request{}
        , m_response{}
        , m_promise{}
        , m_cancel{ false }
    {
        static_assert(std::is_base_of_v<SessionBase<DerivedType>, DerivedType>);

        if (m_options.writeBufferSize().has_value()) m_buffer.reserve(m_options.writeBufferSize().value());
    }

    template <typename DerivedType>
    inline const boost::uuids::uuid& SessionBase<DerivedType>::id() const
    {
        return m_id;
    }

    template <typename DerivedType>
    inline DerivedType& SessionBase<DerivedType>::derived()
    {
        return static_cast<DerivedType&>(*this);
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::run(TRequestImplPtr req, std::promise<Response>& prom)
    {
        m_request = req;
        m_promise = std::move(prom);

        if (m_options.connectionTimeout().has_value())
        {
            boost::beast::get_lowest_layer(derived().stream()).expires_after(std::chrono::milliseconds(m_options.connectionTimeout().value()));
        }

        m_resolver.async_resolve(m_host, m_port, boost::beast::bind_front_handler(&SessionBase::onResolved, derived().shared_from_this()));
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::cancel()
    {
        m_cancel = true;
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::writeRequest()
    {
        auto visitor = overloaded{
            [this](auto&& bodyContent) {
                boost::beast::error_code ec;
                auto request = m_request->prepare(bodyContent, m_options, ec);
                if (nullptr == request) return handleError(ec, "Failed to create request");
                using TBodyType = typename decltype(request)::element_type::body_type;
                auto serializer = std::make_shared<TRequestSerializer<TBodyType>>(*request);
                if (m_options.readBufferSize().has_value()) serializer->writer_impl().buffer_size(m_options.readBufferSize().value());

                if (m_options.sendProgress().has_value() && request->payload_size().has_value())
                {
                    m_sendProgress = m_options.sendProgress().value();
                    m_sendStep = m_options.sendStep().value();
                    m_contentLength = request->payload_size().value();
                    m_progressIndex = 0;
                    m_progressThreshold = 0;
                    m_totalProcessed = 0;
                }

                onWriteSome(request, serializer, ec, 0);
            }
        };

        std::visit(visitor, m_request->m_body.m_content);
    }

    template <typename DerivedType>
    template <typename BodyType>
    inline void SessionBase<DerivedType>::onWriteSome(TRequestPtr<BodyType> request, TRequestSerializerPtr<BodyType> serializer, const boost::beast::error_code& ec, std::size_t bytes_written)
    {
        if (ec) return handleError(ec, "Socket write failed");

        if (m_cancel) return handleCancel();

        if ((bool)m_sendProgress)
        {
            m_totalProcessed += bytes_written;
            if (m_totalProcessed > m_progressThreshold || serializer->is_done())
            {
                ++m_progressIndex;
                m_sendProgress(m_contentLength, m_totalProcessed);
                m_progressThreshold = static_cast<std::size_t>(m_contentLength * m_progressIndex * m_sendStep);
            }
        }

        if (!serializer->is_done())
        {
            if (m_options.requestTimeout().has_value())
            {
                boost::beast::get_lowest_layer(derived().stream()).expires_after(std::chrono::milliseconds(m_options.requestTimeout().value()));
            }

            boost::beast::http::async_write_some(derived().stream(), *serializer, boost::beast::bind_front_handler(&SessionBase::onWriteSome<BodyType>, derived().shared_from_this(), request, serializer));
        }
        else
        {
            m_request->finalize(request);
            onWritten();
        }
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::onResolved(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type results)
    {
        if (ec) return handleError(ec, "Failed to resolve " + m_url);

        if (m_cancel) return handleCancel();

        boost::beast::get_lowest_layer(derived().stream()).async_connect(results, boost::beast::bind_front_handler(&SessionBase::onConnected, derived().shared_from_this()));
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::onConnected(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type)
    {
        if (ec) return handleError(ec, "Failed to connect to " + m_url);

        if (m_cancel) return handleCancel();

        derived().onConnected();
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::onWritten()
    {
        auto parser0 = std::make_shared<TEmptyParser>();

        if (true == m_options.bodyLimit().has_value()) parser0->body_limit(m_options.bodyLimit().value());

        if (m_options.requestTimeout().has_value())
        {
            boost::beast::get_lowest_layer(derived().stream()).expires_after(std::chrono::milliseconds(m_options.requestTimeout().value()));
        }

        boost::beast::http::async_read_header(derived().stream(), m_buffer, *parser0, boost::beast::bind_front_handler(&SessionBase::onHeaderRead, derived().shared_from_this(), parser0));
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::onHeaderRead(TEmptyParserPtr parser0, const boost::beast::error_code& ec, std::size_t)
    {
        if (ec) return handleError(ec, "Socket read header failed");

        if (m_cancel) return handleCancel();

        m_response.setContentType(*parser0);

        auto visitor = overloaded{
            [this, parser0](auto&& bodyContent) {
                boost::beast::error_code ec;
                auto parser = m_response.prepare(bodyContent, parser0, m_options, ec);
                if (nullptr == parser) return handleError(ec, "Failed to create parser");
                using TBodyType = typename decltype(parser)::element_type::value_type::body_type;

                if (m_options.recvProgress().has_value() && parser->content_length().has_value())
                {
                    m_recvProgress = m_options.recvProgress().value();
                    m_recvStep = m_options.recvStep().value();
                    m_contentLength = parser->content_length().value();
                    m_progressIndex = 0;
                    m_progressThreshold = 0;
                    m_totalProcessed = 0;
                }

                onReadSome(parser, ec, 0);
            }
        };

        std::visit(visitor, m_response.m_body.m_content);
    }

    template <typename DerivedType>
    template <typename BodyType>
    inline void SessionBase<DerivedType>::onReadSome(TResponseParserPtr<BodyType> parser, const boost::beast::error_code& ec, std::size_t bytes_read)
    {
        if (ec) return handleError(parser, ec, "Socket read failed");

        if (m_cancel) return handleCancel(parser);

        if ((bool)m_recvProgress)
        {
            m_totalProcessed += bytes_read;
            if (m_totalProcessed > m_progressThreshold || parser->is_done())
            {
                ++m_progressIndex;
                m_recvProgress(m_contentLength, m_totalProcessed);
                m_progressThreshold = static_cast<std::size_t>(m_contentLength * m_progressIndex * m_recvStep);
            }
        }

        if (parser != nullptr && !parser->is_done())
        {
            if (m_options.requestTimeout().has_value())
            {
                boost::beast::get_lowest_layer(derived().stream()).expires_after(std::chrono::milliseconds(m_options.requestTimeout().value()));
            }

            boost::beast::http::async_read_some(derived().stream(), m_buffer, *parser, boost::beast::bind_front_handler(&SessionBase::onReadSome<BodyType>, derived().shared_from_this(), parser));
        }
        else
        {
            onBodyRead<BodyType>(parser);
        }
    }

    template <typename DerivedType>
    template <typename BodyType>
    inline void SessionBase<DerivedType>::onBodyRead(TResponseParserPtr<BodyType> parser)
    {
        closeStream();
        m_response.handleResponse(parser);
        m_promise.set_value(m_response);
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::closeStream()
    {
        boost::beast::error_code ignored;
        boost::beast::get_lowest_layer(derived().stream()).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);

        if (auto clientImpl = m_clientImpl.lock())
        {
            clientImpl->clearSession(m_id);
        }
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::handleCancel()
    {
        closeStream();
        m_response.handleCancel();
        m_promise.set_value(m_response);
    }

    template <typename DerivedType>
    template <typename BodyType>
    inline void SessionBase<DerivedType>::handleCancel(TResponseParserPtr<BodyType> parser)
    {
        closeStream();
        m_response.handleCancel(parser);
        m_promise.set_value(m_response);
    }

    template <typename DerivedType>
    inline void SessionBase<DerivedType>::handleError(const boost::beast::error_code& ec, const std::string& reason)
    {
        closeStream();
        m_response.handleError(ec, reason);
        m_promise.set_value(m_response);
    }

    template <typename DerivedType>
    template <typename BodyType>
    inline void SessionBase<DerivedType>::handleError(TResponseParserPtr<BodyType> parser, const boost::beast::error_code& ec, const std::string& reason)
    {
        closeStream();
        m_response.handleError(parser, ec, reason);
        m_promise.set_value(m_response);
    }
}

#endif

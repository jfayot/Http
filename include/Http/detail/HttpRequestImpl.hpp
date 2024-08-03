#ifndef HTTP_REQUEST_IMPL_HPP_INCLUDED
#define HTTP_REQUEST_IMPL_HPP_INCLUDED

#include "Http/HttpMimeTypes.hpp"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/HttpClientImpl.h"

namespace Http::detail
{
    inline void RequestImpl::header(const std::string &key, const std::string &value)
    {
        m_headers[key] = value;
    }

    inline void RequestImpl::options(const Options& options)
    {
        m_options = options;
    }

    inline void RequestImpl::cancel() const
    {
        m_clientImpl->cancel(m_sessionId);
    }

    RequestImpl::RequestImpl(const std::string& host, const std::string& port, boost::beast::http::verb verb, const std::string &target, TClientImplPtr clientImpl)
        : m_host{ host }
        , m_port{ port }
        , m_verb{ verb }
        , m_target{ target }
        , m_headers{}
        , m_body{}
        , m_clientImpl{ clientImpl }
    {}

    inline TEmptyRequest RequestImpl::req0(const Options& options)
    {
        TEmptyRequest req0{ m_verb, options.proxy().value_or("") + m_target, 11 };
        req0.set(boost::beast::http::field::host, m_host + ":" + m_port);
        req0.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        if (true == options.auth().has_value())
        {
            req0.set(boost::beast::http::field::authorization, options.auth().value());
        }

        for (const auto &[key, value] : m_headers)
        {
            req0.set(key, value);
        }

        return req0;
    }

    inline TEmptyRequestPtr RequestImpl::prepare(const TEmpty&, const Options& options, boost::beast::error_code& ec)
    {
        ec = {};
        return std::make_shared<TEmptyRequest>(req0(options));
    }

    inline TStringRequestPtr RequestImpl::prepare(const std::string& bodyContent, const Options& options, boost::beast::error_code& ec)
    {
        ec = {};
        StringBody::value_type body{ bodyContent };
        TStringRequestPtr request = std::make_shared<TStringRequest>(req0(options));
        request->set(boost::beast::http::field::content_type, ContentApplicationText);
        request->body() = std::move(body);
        request->prepare_payload();
        return request;
    }

    inline TStringRequestPtr RequestImpl::prepare(const nlohmann::json& bodyContent, const Options& options, boost::beast::error_code& ec)
    {
        ec = {};
        StringBody::value_type body{ bodyContent.dump() };
        TStringRequestPtr request = std::make_shared<TStringRequest>(req0(options));
        request->set(boost::beast::http::field::content_type, ContentApplicationJson);
        request->body() = std::move(body);
        request->prepare_payload();
        return request;
    }

    inline TFileRequestPtr RequestImpl::prepare(const fs::path& bodyContent, const Options& options, boost::beast::error_code& ec)
    {
        ec = {};
        FileBody::value_type body;
        body.open(bodyContent.string().c_str(), boost::beast::file_mode::scan, ec);
        if (ec) return nullptr;
        TFileRequestPtr request = std::make_shared<TFileRequest>(req0(options));
        request->set(boost::beast::http::field::content_type, mime_type(bodyContent));
        request->body() = std::move(body);
        request->prepare_payload();

        return request;
    }

    inline TFormRequestPtr RequestImpl::prepare(const FormData& bodyContent, const Options& options, boost::beast::error_code& ec)
    {
        ec = {};
        FormDataBody::value_type body{ bodyContent };
        TFormRequestPtr request = std::make_shared<TFormRequest>(req0(options));
        request->set(boost::beast::http::field::content_type, ContentMultipartFormData);
        request->body() = std::move(body);
        request->prepare_payload();

        return request;
    }

    inline std::string RequestImpl::dump() const
    {
        //TODO add headers and synthetic body dump
        return std::string(boost::beast::http::to_string(m_verb)) + " " + m_target;
    }
}

#endif

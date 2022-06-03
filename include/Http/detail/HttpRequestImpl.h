#ifndef HTTP_REQUEST_IMPL_H_INCLUDED
#define HTTP_REQUEST_IMPL_H_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/HttpBody.hpp"
#include "Http/HttpResponse.hpp"
#include "Http/detail/empty_body.hpp"
#include "Http/detail/basic_string_body.hpp"
#include "Http/detail/basic_file_body.hpp"
#include "Http/detail/basic_multipart_body.hpp"

namespace Http::detail
{
    class RequestImpl
    {
    private:
        friend struct ClientImpl;
        friend class Http::Request;
        template <typename DerivedType>
        friend class SessionBase;

        using Headers = std::map<std::string, std::string>;

        std::string m_host;
        std::string m_port;
        boost::beast::http::verb m_verb;
        std::string m_target;
        Options m_options;
        Headers m_headers;
        Body m_body;
        TClientImplPtr m_clientImpl;
        boost::uuids::uuid m_sessionId;

    public:
        RequestImpl() = delete;
        RequestImpl(const std::string& host, const std::string& port, boost::beast::http::verb verb, const std::string& target, TClientImplPtr clientImpl);
        virtual ~RequestImpl() = default;
        RequestImpl(const RequestImpl& other) = delete;
        RequestImpl &operator=(const RequestImpl& other) = delete;
        RequestImpl(RequestImpl&& other) = delete;
        RequestImpl &operator=(RequestImpl&& other) = delete;

        void header(const std::string& key, const std::string& value);
        void options(const Options& options);
        const Options& options() const { return m_options; }
        const Body& body() { return m_body; }
        template <typename ContentType>
        void body(ContentType&& content) { m_body = content; }

        void cancel() const;

        std::string dump() const;

    private:
        TEmptyRequest req0(const Options& options);

        TEmptyRequestPtr prepare(const TEmpty& bodyContent, const Options& options, boost::beast::error_code& ec);
        TStringRequestPtr prepare(const std::string& bodyContent, const Options& options, boost::beast::error_code& ec);
        TStringRequestPtr prepare(const nlohmann::json& bodyContent, const Options& options, boost::beast::error_code& ec);
        TFileRequestPtr prepare(const fs::path& bodyContent, const Options& options, boost::beast::error_code& ec);
        TFormRequestPtr prepare(const FormData& bodyContent, const Options& options, boost::beast::error_code& ec);

        template <typename BodyType>
        void finalize(TRequestPtr<BodyType> &request)
        {}

        void finalize(TRequestPtr<FileBody> &request)
        {
            if (request != nullptr) request->body().close();
        }
    };
}

#endif

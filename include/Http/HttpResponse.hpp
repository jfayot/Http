#ifndef HTTP_RESPONSE_HPP_INCLUDED
#define HTTP_RESPONSE_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/HttpBody.hpp"
#include "Http/detail/HttpFile.hpp"
#include "Http/detail/empty_body.hpp"
#include "Http/detail/basic_string_body.hpp"
#include "Http/detail/basic_file_body.hpp"
#include "Http/detail/basic_multipart_body.hpp"

namespace Http
{
    static constexpr char const *ContentText = "text";
    static constexpr char const *ContentApplicationText = "application/text";
    static constexpr char const *ContentApplicationJson = "application/json";
    static constexpr char const *ContentApplicationXml = "application/xml";
    static constexpr char const *ContentApplicationOctetStream = "application/octet-stream";
    static constexpr char const *ContentMultipartFormData = "multipart/form-data";

    class Response
    {
    private:
        template <typename DerivedType>
        friend class detail::SessionBase;

        unsigned int m_status = castFromEnum(boost::beast::http::status::unknown);
        std::string m_reason;
        Body m_body;
        fs::path m_tempPath;

    public:
        unsigned int status() const { return m_status; }
        const std::string &reason() const { return m_reason; }
        const Body &body() const { return m_body; }

        bool ok() const
        {
            return castFromEnum(boost::beast::http::status::ok) == m_status;
        }

        Response() = default;
        virtual ~Response() = default;
        Response(const Response &other) = default;
        Response &operator=(const Response &other) = default;
        Response(Response &&other) = default;
        Response &operator=(Response &&other) = default;

    private:
        void setContentType(const detail::TEmptyParser &parser)
        {
            auto contentType = parser.get()[boost::beast::http::field::content_type];

            if (contentType.length() == 0)
            {
                m_body = TEmpty{};
            }
            else if (
                true == boost::algorithm::contains(contentType, ContentApplicationText) ||
                true == boost::algorithm::starts_with(contentType, ContentText))
            {
                m_body = std::string{};
            }
            else if (true == boost::algorithm::contains(contentType, ContentApplicationJson))
            {
                m_body = nlohmann::json{};
            }
            else if (true == boost::algorithm::contains(contentType, ContentApplicationXml))
            {
                // TODO
                // m_body = TXml{};
            }
            else if (true == boost::algorithm::contains(contentType, ContentMultipartFormData))
            {
                // TODO
                m_body = FormData{};
            }
            else
            {
                m_body = fs::path{};
            }
        }

        detail::TEmptyParserPtr prepare(const TEmpty &bodyContent, detail::TEmptyParserPtr parser0, const Options &options, boost::beast::error_code &ec)
        {
            ec = {};
            return parser0;
        }

        detail::TStringParserPtr prepare(const std::string &bodyContent, detail::TEmptyParserPtr parser0, const Options &options, boost::beast::error_code &ec)
        {
            ec = {};
            return std::make_shared<detail::TStringParser>(std::move(*parser0));
        }

        detail::TStringParserPtr prepare(const nlohmann::json &bodyContent, detail::TEmptyParserPtr parser0, const Options &options, boost::beast::error_code &ec)
        {
            ec = {};
            return std::make_shared<detail::TStringParser>(std::move(*parser0));
        }

        detail::TFileParserPtr prepare(const fs::path &bodyContent, detail::TEmptyParserPtr parser0, const Options &options, boost::beast::error_code &ec)
        {
            ec = {};
            auto parser = std::make_shared<detail::TFileParser>(std::move(*parser0));
            m_tempPath = options.fileOut().value_or(options.tempDir().value_or(std::filesystem::temp_directory_path()) / boost::uuids::to_string(boost::uuids::random_generator()()));
            detail::FileBody::value_type body;
            boost::beast::file_mode mode = options.fileAppend().value_or(false)
                                               ? (std::filesystem::exists(m_tempPath)
                                                      ? boost::beast::file_mode::append_existing
                                                      : boost::beast::file_mode::append)
                                               : boost::beast::file_mode::append;
            body.open(m_tempPath.string().c_str(), mode, ec);
            if (ec)
                return nullptr;
            body.seek(body.size(), ec);
            if (ec)
                return nullptr;
            if (options.writeBufferSize().has_value())
                body.buffer_size(options.writeBufferSize().value());
            parser->get().body() = std::move(body);
            return parser;
        }

        detail::TFormParserPtr prepare(const FormData &bodyContent, detail::TEmptyParserPtr parser0, const Options &options, boost::beast::error_code &ec)
        {
            ec = {};
            return nullptr;
        }

        template <typename BodyType>
        void getStatus(detail::TResponseParserPtr<BodyType> &parser)
        {
            m_status = parser->get().result_int();
            m_reason = std::string{parser->get().reason()};
        }

        void handleCancel()
        {
            m_reason = "Request canceled";
            m_status = castFromEnum(boost::beast::http::status::client_closed_request);
        }

        void handleError(const boost::beast::error_code& ec, const std::string& reason)
        {
            m_reason = reason + "; " + ec.message();
        }

        template <typename BodyType>
        void handleResponse(detail::TResponseParserPtr<BodyType> &parser)
        {
        }

        void handleResponse(detail::TResponseParserPtr<detail::EmptyBody> &parser)
        {
            getStatus(parser);
        }

        void handleResponse(detail::TResponseParserPtr<detail::StringBody> &parser)
        {
            getStatus(parser);

            auto visitor = overloaded{
                [](auto &&) {},
                [&parser](std::string &bodyContent)
                {
                    bodyContent = std::move(parser->get().body());
                },
                [&parser](nlohmann::json &bodyContent)
                {
                    if (true == nlohmann::json::accept(parser->get().body()))
                        bodyContent = std::move(nlohmann::json::parse(parser->get().body()));
                    else
                        bodyContent = nullptr;
                }};
            std::visit(visitor, m_body.m_content);
        }

        void handleResponse(detail::TResponseParserPtr<detail::FileBody> &parser)
        {
            getStatus(parser);
            if (parser != nullptr)
                parser->get().body().close();

            m_body.m_content = m_tempPath;
        }

        void handleResponse(detail::TResponseParserPtr<detail::FormDataBody> &parser)
        {
            // TODO
        }

        template <typename BodyType>
        void handleCancel(detail::TResponseParserPtr<BodyType> &parser)
        {
            handleCancel();
        }

        void handleCancel(detail::TResponseParserPtr<detail::FileBody> &parser)
        {
            handleCancel();
            std::error_code ignored;
            if (parser != nullptr)
            {
                parser->get().body().close();
                parser->get().body().remove(ignored);
            }
        }

        template <typename BodyType>
        void handleError(detail::TResponseParserPtr<BodyType> &parser, const boost::beast::error_code &ec, const std::string &reason)
        {
            handleError(ec, reason);
        }

        void handleError(detail::TResponseParserPtr<detail::FileBody> &parser, const boost::beast::error_code &ec, const std::string &reason)
        {
            handleError(ec, reason);
            if (parser != nullptr)
                parser->get().body().close();
        }
    };
}

#endif

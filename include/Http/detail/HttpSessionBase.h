#ifndef HTTP_SESSION_BASE_H_INCLUDED
#define HTTP_SESSION_BASE_H_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpRequest.hpp"
#include "Http/HttpResponse.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/detail/HttpISession.hpp"

namespace Http::detail
{
    template <typename DerivedType>
    class SessionBase : public ISession
    {
    private:
        boost::uuids::uuid m_id;
        TClientImplWeakPtr m_clientImpl;
        Options m_options;
        std::string m_host;
        std::string m_port;
        std::string m_url;
        boost::asio::ip::tcp::resolver m_resolver;
        boost::beast::flat_buffer m_buffer;
        TRequestImplPtr m_request;
        Response m_response;
        std::promise<Response> m_promise;
        TProgressCb m_sendProgress;
        TProgressCb m_recvProgress;
        double m_sendStep;
        double m_recvStep;
        unsigned int m_progressIndex;
        std::size_t m_contentLength;
        std::size_t m_totalProcessed;
        std::size_t m_progressThreshold;

    protected:
        bool m_cancel;

    public:
        SessionBase() = delete;
        SessionBase(TClientImplPtr clientImpl, boost::asio::io_context& ioc, const std::string& host, const std::string& port, const Options& options);
        virtual ~SessionBase() = default;
        SessionBase(const SessionBase &other) = delete;
        SessionBase &operator=(const SessionBase &other) = delete;
        SessionBase(SessionBase &&other) = delete;
        SessionBase &operator=(SessionBase &&other) = delete;


        const boost::uuids::uuid& id() const override;
        void run(TRequestImplPtr req, std::promise<Response>& prom) override;
        void cancel() override;

    protected:
        void handleError(const boost::beast::error_code& ec, const std::string& reason);
        void handleCancel();
        template <typename BodyType>
        void handleError(TResponseParserPtr<BodyType> parser, const boost::beast::error_code& ec, const std::string& reason);
        template <typename BodyType>
        void handleCancel(TResponseParserPtr<BodyType> parser);
        void writeRequest();

    private:
        [[nodiscard]]
        DerivedType& derived();

        void onResolved(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type results);
        void onConnected(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
        void onWritten();
        void onHeaderRead(TEmptyParserPtr parser0, const boost::beast::error_code& ec, std::size_t);
        template <typename BodyType>
        void onBodyRead(TResponseParserPtr<BodyType> parser);

        void closeStream();

        template <typename BodyType>
        void onWriteSome(TRequestPtr<BodyType> request, TRequestSerializerPtr<BodyType> serializer, const boost::beast::error_code& ec, std::size_t bytes_written);

        template <typename BodyType>
        void onReadSome(TResponseParserPtr<BodyType> parser, const boost::beast::error_code& ec, std::size_t bytes_read);
    };
}

#endif

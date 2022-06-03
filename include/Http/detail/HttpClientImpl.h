#ifndef HTTP_CLIENT_IMPL_H_INCLUDED
#define HTTP_CLIENT_IMPL_H_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/HttpRequest.hpp"
#include "Http/HttpResponse.hpp"

namespace Http::detail
{
    struct ISession;

    struct ClientImpl : public std::enable_shared_from_this<ClientImpl>
    {
    private:
        friend class Http::Client;

        using TMutex = std::mutex;
        using TLock = std::lock_guard<TMutex>;
        using TThreads = std::vector<std::thread>;
        using TWorkguard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        using TSessionPtr = std::shared_ptr<ISession>;
        using TSessionPtrs = std::map<boost::uuids::uuid, TSessionPtr>;

        static constexpr char const * const DefaultPlainPort = "80";
        static constexpr char const * const DefaultSslPort = "443";
        static constexpr char const * const Https = "https";
        static constexpr char const * const HostPortScheme = R"(^(?:(http|https):\/\/)?([a-z0-9\-\.]+)(?::(\d+))?$)";

        mutable boost::asio::io_context m_ioc;

        TWorkguard m_workguard;
        TThreads m_threads;
        mutable TMutex m_mutex;

        std::string m_url;
        std::string m_host;
        std::string m_port;
        bool m_isSsl;
        Options m_options;
        mutable TSessionPtrs m_sessions;

    public:
        ClientImpl() = delete;
        ClientImpl(const std::string &url, Options options);
        virtual ~ClientImpl();
        ClientImpl(const ClientImpl& other) = delete;
        ClientImpl &operator=(const ClientImpl& other) = delete;
        ClientImpl(ClientImpl&& other) = delete;
        ClientImpl &operator=(ClientImpl&& other) = delete;

        Request get(const std::string& target) const;
        Request post(const std::string& target) const;
        Request put(const std::string& target) const;
        Request patch(const std::string& target) const;
        Request delete_(const std::string& target) const;
        Request get(const std::stringstream& target) const;

        Request post(const std::stringstream& target) const;
        Request put(const std::stringstream& target) const;
        Request patch(const std::stringstream& target) const;
        Request delete_(const std::stringstream& target) const;

        void cancel(const boost::uuids::uuid& id) const;
        void cancelAll() const;
        void shutdown();
        void clearSession(const boost::uuids::uuid& id) const;

        std::shared_future<Response> send(TRequestImplPtr req) const;
    };
}

#endif

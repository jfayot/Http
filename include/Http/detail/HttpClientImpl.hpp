#ifndef HTTP_CLIENT_IMPL_HPP_INCLUDED
#define HTTP_CLIENT_IMPL_HPP_INCLUDED

#include "Http/detail/HttpClientImpl.h"
#include "Http/detail/HttpRequestImpl.h"
#include "Http/detail/HttpSessionPlain.hpp"
#include "Http/detail/HttpSessionSsl.hpp"

namespace Http::detail
{
    ClientImpl::ClientImpl(const std::string &url, Options options)
        : m_ioc{}
        , m_workguard{ m_ioc.get_executor() }
        , m_threads{}
        , m_mutex{}
        , m_url{ boost::algorithm::to_lower_copy(url) }
        , m_host{}
        , m_port{ DefaultPlainPort }
        , m_isSsl{ false }
        , m_options{ options }
        , m_sessions{}
    {
        auto nbThreads = m_options.nbThreads().value_or(Options::DefaultNbThreads);
        m_threads.reserve(nbThreads);
        for (auto i = 0u; i < nbThreads; ++i)
        {
            m_threads.emplace_back([this] { m_ioc.run(); });
        }

        std::regex schemeHostPort{ HostPortScheme, std::regex_constants::ECMAScript | std::regex_constants::icase };

        std::smatch match;
        if (true == std::regex_match(url, match, schemeHostPort))
        {
            if (Https == match[1].str()) m_isSsl = true;
            m_host = match[2].str();
            m_port = m_isSsl ? DefaultSslPort : DefaultPlainPort;
            if (!match[3].str().empty()) m_port = match[3].str();
        }
    }

    ClientImpl::~ClientImpl()
    {
        shutdown();
    }

    inline Request ClientImpl::get(const std::string& target) const
    {
        return Request{ m_host, m_port, boost::beast::http::verb::get, target, shared_from_this() };
    }

    inline Request ClientImpl::post(const std::string& target) const
    {
        return Request{ m_host, m_port, boost::beast::http::verb::post, target, shared_from_this() };
    }

    inline Request ClientImpl::put(const std::string& target) const
    {
        return Request{ m_host, m_port, boost::beast::http::verb::put, target, shared_from_this() };
    }

    inline Request ClientImpl::patch(const std::string& target) const
    {
        return Request{ m_host, m_port, boost::beast::http::verb::patch, target, shared_from_this() };
    }

    inline Request ClientImpl::delete_(const std::string& target) const
    {
        return Request{ m_host, m_port, boost::beast::http::verb::delete_, target, shared_from_this() };
    }

    inline Request ClientImpl::get(const std::stringstream& target) const
    {
        return get(target.str());
    }

    inline Request ClientImpl::post(const std::stringstream& target) const
    {
        return post(target.str());
    }

    inline Request ClientImpl::put(const std::stringstream& target) const
    {
        return put(target.str());
    }

    inline Request ClientImpl::patch(const std::stringstream& target) const
    {
        return patch(target.str());
    }

    inline Request ClientImpl::delete_(const std::stringstream& target) const
    {
        return delete_(target.str());
    }

    inline void ClientImpl::cancel(const boost::uuids::uuid& id) const
    {
        TLock lock{ m_mutex };
        auto it = m_sessions.find(id);
        if (it != m_sessions.end()) it->second->cancel();
    }

    inline void ClientImpl::cancelAll() const
    {
        TLock lock{ m_mutex };
        for (auto&[id, session] : m_sessions)
        {
            session->cancel();
        }
    }

    inline void ClientImpl::shutdown()
    {
        m_ioc.stop();
        m_workguard.reset();
        for (auto& thread : m_threads) thread.join();
    }

    inline void ClientImpl::clearSession(const boost::uuids::uuid& id) const
    {
        TLock lock{ m_mutex };
        m_sessions.erase(id);
    }

    inline std::shared_future<Response> ClientImpl::send(TRequestImplPtr req) const
    {
        std::promise<Response> prom;
        auto res = prom.get_future();

        auto options = m_options + req->options();

        TSessionPtr session;
        if (m_isSsl)
        {
            session = std::make_shared<SessionSsl>(shared_from_this(), m_ioc, m_host, m_port, options);
        }
        else
        {
            session = std::make_shared<SessionPlain>(shared_from_this(), m_ioc, m_host, m_port, options);
        }

        if (nullptr != session)
        {
            TLock lock{ m_mutex };
            req->m_sessionId = session->id();
            m_sessions[session->id()] = session;
            session->run(req, prom);
        }
        else
        {
            prom.set_value(Response{});
        }

        return res;
    }
}

#endif

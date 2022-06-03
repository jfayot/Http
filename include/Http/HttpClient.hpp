#ifndef HTTP_CLIENT_HPP_INCLUDED
#define HTTP_CLIENT_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"
#include "Http/HttpRequest.hpp"
#include "Http/HttpResponse.hpp"
#include "Http/detail/HttpClientImpl.h"

namespace Http
{
    class Client
    {
    private:
        detail::TClientImplPtr m_clientImpl;

    public:
        Client() = delete;
        Client(const std::string &url, Options options = {})
            : m_clientImpl{std::make_shared<detail::ClientImpl>(url, options)}
        {
            if (nullptr == m_clientImpl)
            {
                throw std::bad_alloc();
            }
        }
        virtual ~Client() = default;
        Client(const Client &other) = delete;
        Client &operator=(const Client &other) = delete;
        Client(Client &&other) = delete;
        Client &operator=(Client &&other) = delete;

        Request get(const std::string &target) const
        {
            return m_clientImpl->get(target);
        }

        Request post(const std::string &target) const
        {
            return m_clientImpl->post(target);
        }

        Request put(const std::string &target) const
        {
            return m_clientImpl->put(target);
        }

        Request patch(const std::string &target) const
        {
            return m_clientImpl->patch(target);
        }

        Request delete_(const std::string &target) const
        {
            return m_clientImpl->delete_(target);
        }

        Request get(const std::stringstream &target) const
        {
            return m_clientImpl->get(target);
        }

        Request post(const std::stringstream &target) const
        {
            return m_clientImpl->post(target);
        }

        Request put(const std::stringstream &target) const
        {
            return m_clientImpl->put(target);
        }

        Request patch(const std::stringstream &target) const
        {
            return m_clientImpl->patch(target);
        }

        Request delete_(const std::stringstream &target) const
        {
            return m_clientImpl->delete_(target);
        }

        void cancelAll() const
        {
            m_clientImpl->cancelAll();
        }
    };
}

#endif

#ifndef HTTP_ISESSION_HPP_INCLUDED
#define HTTP_ISESSION_HPP_INCLUDED

#include "Http/HttpForwards.hpp"

namespace Http::detail
{
    struct ISession
    {
        virtual ~ISession() = default;

        virtual const boost::uuids::uuid& id() const = 0;
        virtual void run(TRequestImplPtr req, std::promise<Response>& prom) = 0;
        virtual void cancel() = 0;
    };
}

#endif

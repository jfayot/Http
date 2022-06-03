#ifndef HTTP_AUTH_HPP_INCLUDED
#define HTTP_AUTH_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/detail/Base64.hpp"

namespace Http
{
    enum class AuthType
    {
        Bearer,
        Basic
    };

    template <AuthType auth>
    struct Auth
    {
    };

    template <>
    struct Auth<AuthType::Bearer>
    {
        Auth<AuthType::Bearer>(const std::string &token) : value{ "Bearer " + token } {}
        std::string value;
    };

    template <>
    struct Auth<AuthType::Basic>
    {
        Auth<AuthType::Basic>(const std::string &login, const std::string &password) : value{ "Basic " + Http::detail::Base64::encode(login + ":" + password) } {}
        std::string value;
    };
}

#endif

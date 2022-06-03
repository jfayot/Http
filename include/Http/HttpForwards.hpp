#ifndef HTTP_FORWARDS_HPP_INCLUDED
#define HTTP_FORWARDS_HPP_INCLUDED

#include <nlohmann/json.hpp>

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <future>
#include <functional>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <system_error>
#include <map>
#include <variant>
#include <string>
#include <sstream>
#include <optional>
#include <regex>
#include <type_traits>
#include <filesystem>

namespace fs = std::filesystem;

namespace Http
{
    template <typename E>
    constexpr auto castFromEnum(E value) -> typename std::underlying_type<E>::type
    {
        return static_cast<typename std::underlying_type<E>::type>(value);
    }

    constexpr auto operator ""_KiB(long double kib) -> long double
    {
        return (kib * (1 << 10));
    }

    constexpr auto operator ""_MiB(long double mib) -> long double
    {
        return (mib * (1 << 20));
    }

    constexpr auto operator ""_GiB(long double gib) -> long double
    {
        return (gib * (1 << 30));
    }

    constexpr auto operator ""_KiB(unsigned long long kib) -> unsigned long
    {
        return (kib * (1 << 10));
    }

    constexpr auto operator ""_MiB(unsigned long long mib) -> unsigned long
    {
        return (mib * (1 << 20));
    }

    constexpr auto operator ""_GiB(unsigned long long gib) -> unsigned long
    {
        return (gib * (1 << 30));
    }

    template <class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };
    template <class... Ts>
    overloaded(Ts...)->overloaded<Ts...>;

    class Client;
    class Request;
    class Response;
    class FormData;

    namespace detail
    {
        template <typename Body>
        using TRequest = boost::beast::http::request<Body>;
        template <typename Body>
        using TRequestPtr = std::shared_ptr<TRequest<Body>>;

        template <typename Body>
        using TResponseParser = boost::beast::http::response_parser<Body>;
        template <typename Body>
        using TResponseParserPtr = std::shared_ptr<boost::beast::http::response_parser<Body>>;

        template <typename Body>
        using TRequestSerializer = boost::beast::http::request_serializer<Body>;
        template <typename Body>
        using TRequestSerializerPtr = std::shared_ptr<TRequestSerializer<Body>>;

        struct ClientImpl;
        class RequestImpl;

        template <typename DerivedType>
        class SessionBase;

        class HttpFile;

        using TClientImplPtr = std::shared_ptr<const ClientImpl>;
        using TClientImplWeakPtr = std::weak_ptr<const ClientImpl>;
        using TRequestImplPtr = std::shared_ptr<RequestImpl>;
    }

    struct Empty {};
    using TEmpty = Empty;
}

#endif

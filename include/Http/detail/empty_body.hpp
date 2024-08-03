#ifndef EMPTY_BODY_HPP_INCLUDED
#define EMPTY_BODY_HPP_INCLUDED

#include "Http/HttpForwards.hpp"

namespace Http::detail
{
    struct empty_body
    {
        struct value_type {};

        struct reader
        {
            template<bool isRequest, class Fields>
            explicit reader(boost::beast::http::header<isRequest, Fields>&, value_type&)
            {
            }

            void init(boost::optional<std::uint64_t> const&, boost::beast::error_code& ec)
            {
                ec = {};
            }

            template<class ConstBufferSequence>
            std::size_t put(ConstBufferSequence const&, boost::beast::error_code& ec)
            {
                ec = boost::beast::http::error::unexpected_body;
                return 0;
            }

            void finish(boost::beast::error_code& ec)
            {
                ec = {};
            }
        };

        struct writer
        {
            using const_buffers_type = boost::asio::const_buffer;

            template<bool isRequest, class Fields>
            explicit writer(boost::beast::http::header<isRequest, Fields> const&, value_type const&)
            {
            }

            void buffer_size(std::size_t size)
            {
                //Not useful here
            }

            void init(boost::beast::error_code& ec)
            {
                ec = {};
            }

            boost::optional<std::pair<const_buffers_type, bool>> get(boost::beast::error_code& ec)
            {
                ec = {};
                return boost::none;
            }
        };

        static uint64_t size(value_type)
        {
            return 0;
        }
    };

    using EmptyBody = empty_body;
    using TEmptyRequest = TRequest<EmptyBody>;
    using TEmptyRequestPtr = TRequestPtr<EmptyBody>;
    using TEmptyParser = TResponseParser<EmptyBody>;
    using TEmptyParserPtr = std::shared_ptr<TEmptyParser>;
}

#endif

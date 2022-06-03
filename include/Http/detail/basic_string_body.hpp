#ifndef BASIC_STRING_BODY_HPP_INCLUDED
#define BASIC_STRING_BODY_HPP_INCLUDED

#include "Http/HttpForwards.hpp"

namespace Http::detail
{
    template <class CharType>
    struct basic_string_body
    {
        using value_type = std::basic_string<CharType>;
        using Traits = std::char_traits<CharType>;

        class reader
        {
        private:
            value_type& m_body;

        public:
            template<bool isRequest, class Fields>
            explicit reader(boost::beast::http::header<isRequest, Fields>&, value_type& body)
                : m_body(body)
            {
            }

            void init(boost::optional<unsigned long> const& length, boost::beast::error_code& ec)
            {
                if (length)
                {
                    if(*length > m_body.max_size())
                    {
                        ec = boost::beast::http::error::buffer_overflow;
                        return;
                    }

                    m_body.reserve(boost::beast::detail::clamp(*length));
                }

                ec = {};
            }

            template<class ConstBufferSequence>
            std::size_t put(ConstBufferSequence const& buffers, boost::beast::error_code& ec)
            {
                auto const extra = boost::beast::buffer_bytes(buffers);
                auto const size = m_body.size();
                if (extra > m_body.max_size() - size)
                {
                    ec = boost::beast::http::error::buffer_overflow;
                    return 0;
                }

                m_body.resize(size + extra);
                ec = {};
                CharType* dest = &m_body[size];
                for (auto b : boost::beast::buffers_range_ref(buffers))
                {
                    Traits::copy(dest, static_cast<CharType const*>(b.data()), b.size());
                    dest += b.size();
                }

                return extra;
            }

            void finish(boost::beast::error_code& ec)
            {
                ec = {};
            }
        };

        class writer
        {
            value_type const& m_body;

        public:
            using const_buffers_type = boost::asio::const_buffer;

            template<bool isRequest, class Fields>
            explicit writer(boost::beast::http::header<isRequest, Fields> const&, value_type const& b)
                : m_body(b)
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
                return { { const_buffers_type{ m_body.data(), m_body.size() }, false} };
            }
        };

        static std::size_t size(value_type const& body)
        {
            return body.size();
        }
    };

    using StringBody = basic_string_body<char>;
    using TStringRequest = TRequest<StringBody>;
    using TStringRequestPtr = TRequestPtr<StringBody>;
    using TStringParser = TResponseParser<StringBody>;
    using TStringParserPtr = std::shared_ptr<TStringParser>;
}

#endif

#ifndef BASIC_FILE_BODY_HPP_INCLUDED
#define BASIC_FILE_BODY_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"

namespace Http::detail
{
    template <class FileType>
    struct basic_file_body
    {
        using value_type = FileType;

        class reader
        {
        private:
            value_type& m_body;

        public:
            template<bool isRequest, class Fields>
            explicit reader(boost::beast::http::header<isRequest, Fields>&, value_type& body)
                : m_body{ body }
            {
            }

            void init(boost::optional<uint64_t> const&, boost::beast::error_code& ec)
            {
                ec = {};
            }

            template<class ConstBufferSequence>
            std::size_t put(ConstBufferSequence const& buffers, boost::beast::error_code& ec)
            {
                std::size_t nbWritten = 0;

                for (auto buffer : boost::beast::buffers_range_ref(buffers))
                {
                    nbWritten += m_body.write(buffer.data(), buffer.size(), ec);
                    if (ec) return nbWritten;
                }

                ec = {};
                return nbWritten;
            }

            void finish(boost::beast::error_code& ec)
            {
                m_body.finalize(ec);
            }
        };

        class writer
        {
        private:
            value_type& m_body;
            std::size_t m_remaining;
            std::size_t m_read_buffer_size;
            char* m_read_buffer;

        public:
            using const_buffers_type = boost::asio::const_buffer;

            template<bool isRequest, class Fields>
            writer(boost::beast::http::header<isRequest, Fields>&, value_type& body)
                : m_body{ body }
                , m_read_buffer_size{ Options::DefaultBufferSize }
                , m_read_buffer{ new char[Options::DefaultBufferSize] }
            {
                m_remaining = body.size();
            }

            virtual ~writer()
            {
                delete[] m_read_buffer;
            }

            void buffer_size(std::size_t size)
            {
                delete[] m_read_buffer;
                m_read_buffer_size = size;
                m_read_buffer = new char[size];
            }

            void init(boost::beast::error_code& ec)
            {
                ec = {};
            }

            boost::optional<std::pair<const_buffers_type, bool>> get(boost::beast::error_code& ec)
            {
                auto const amount = m_remaining > m_read_buffer_size ? m_read_buffer_size : m_remaining;

                if (0 == amount)
                {
                    ec = {};
                    return boost::none;
                }

                auto const nbRead = m_body.read(m_read_buffer, amount, ec);
                if (ec)
                {
                    return boost::none;
                }

                if (nbRead == 0)
                {
                    ec = boost::beast::http::error::short_read;
                    return boost::none;
                }

                m_remaining -= nbRead;

                ec = {};
                return { { const_buffers_type{ m_read_buffer, nbRead }, m_remaining > 0 } };
            }
        };

        static uint64_t size(value_type const& body)
        {
            return body.size();
        }
    };

    using FileBody = basic_file_body<HttpFile>;
    using TFileRequest = TRequest<FileBody>;
    using TFileRequestPtr = TRequestPtr<FileBody>;
    using TFileParser = TResponseParser<FileBody>;
    using TFileParserPtr = std::shared_ptr<TFileParser>;
}

#endif

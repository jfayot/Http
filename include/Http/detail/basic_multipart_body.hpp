#ifndef BASIC_MULTIPART_BODY_HPP_INCLUDED
#define BASIC_MULTIPART_BODY_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"

namespace Http::detail
{
    template <typename MultipartType>
    struct basic_multipart_body
    {
        using value_type = MultipartType;

        //TODO the reader will be required if some day we need to handle FormData responses from the server (which is not the case right now, only posting FormData)
        class reader
        {
        public:
            template<bool isRequest, class Fields>
            explicit reader(boost::beast::http::header<isRequest, Fields>&, value_type&) {}

            void init(boost::optional<unsigned long> const&, boost::beast::error_code&) {}

            template<class ConstBufferSequence>
            std::size_t put(ConstBufferSequence const&, boost::beast::error_code&) { return 0; }

            void finish(boost::beast::error_code&) {}
        };

        class writer
        {
        private:
            using TFile = boost::beast::file;
            enum class MultipartStep
            {
                Parameters,
                FileHeader,
                FileData,
                ClosingBoundary,
            };
            value_type &m_body;
            MultipartStep m_step = MultipartStep::Parameters;
            TFile m_file;
            unsigned long m_remainingFiles;
            unsigned int m_fileIndex{ 0 };
            unsigned long m_remainingBytes;
            std::size_t m_read_buffer_size;
            char* m_read_buffer;

        public:
            using const_buffers_type = boost::asio::const_buffer;

            template <bool isRequest, class Fields>
            writer(boost::beast::http::header<isRequest, Fields> &header, value_type &body)
                : m_body(body)
                , m_read_buffer_size{ Options::DefaultBufferSize }
                , m_read_buffer{ new char[Options::DefaultBufferSize] }
            {
                auto contentType = std::string{ header[boost::beast::http::field::content_type] } +"; " + m_body.m_multipart;
                header.set(boost::beast::http::field::content_type, contentType);
                m_remainingFiles = m_body.m_files.size();
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

            void init(boost::beast::error_code &ec)
            {
                ec = {};
            }

            boost::optional<std::pair<const_buffers_type, bool>> get(boost::beast::error_code &ec)
            {
                switch (m_step)
                {
                case MultipartStep::Parameters:
                {
                    ec = {};
                    if (0 == m_remainingFiles)
                        m_step = MultipartStep::ClosingBoundary;
                    else
                        m_step = MultipartStep::FileHeader;

                    return { {const_buffers_type{ m_body.m_parameters.data(), m_body.m_parameters.size()}, true} };
                }
                case MultipartStep::FileHeader:
                {
                    ec = {};
                    m_file.open(m_body.m_files[m_fileIndex].m_path.string().c_str(), boost::beast::file_mode::read, ec);
                    if (ec) return boost::none;

                    m_file.seek(m_body.m_files[m_fileIndex].m_chunkOffset, ec);
                    if (ec) return boost::none;

                    m_remainingBytes = m_body.m_files[m_fileIndex].m_chunkSize;
                    m_step = MultipartStep::FileData;

                    return { {const_buffers_type{ m_body.m_files[m_fileIndex].m_partHeader.data(), m_body.m_files[m_fileIndex].m_partHeader.size()}, true} };
                }
                case MultipartStep::FileData:
                {
                    auto const amount = m_remainingBytes > m_read_buffer_size ? m_read_buffer_size : static_cast<std::size_t>(m_remainingBytes); // Handle the case where the file is zero length
                    if (amount == 0)
                    {
                        ec = {};
                        nextFile();
                        return { {const_buffers_type{ m_read_buffer, 0}, true} };
                    }
                    auto const nread = m_file.read(m_read_buffer, amount, ec);
                    if (ec)
                    {
                        boost::beast::error_code ignored;
                        m_file.close(ignored);
                        return boost::none;
                    }
                    if (nread == 0)
                    {
                        ec = boost::beast::http::error::short_read;
                        return boost::none;
                    }
                    m_remainingBytes -= nread;

                    nextFile();

                    ec = {};
                    return { {const_buffers_type{ m_read_buffer, nread}, true} };
                }
                case MultipartStep::ClosingBoundary:
                {
                    ec = {};
                    return { {const_buffers_type{ m_body.m_closing.data(), m_body.m_closing.size()}, false} };
                }
                default:
                {
                    ec = boost::beast::http::error::unexpected_body;
                    return boost::none;
                }
                }
            }

        private:
            void nextFile()
            {
                if (0 == m_remainingBytes)
                {
                    boost::beast::error_code ignored;
                    m_file.close(ignored);
                    --m_remainingFiles;
                    if (0 == m_remainingFiles)
                    {
                        m_step = MultipartStep::ClosingBoundary;
                    }
                    else
                    {
                        ++m_fileIndex;
                        m_step = MultipartStep::FileHeader;
                    }
                }
            }
        };

        static unsigned long size(value_type const& body)
        {
            return body.size();
        }
    };

    using FormDataBody = detail::basic_multipart_body<FormData>;
    using TFormRequest = TRequest<FormDataBody>;
    using TFormRequestPtr = TRequestPtr<FormDataBody>;
    using TFormParser = TResponseParser<FormDataBody>;
    using TFormParserPtr = std::shared_ptr<TFormParser>;
}

#endif

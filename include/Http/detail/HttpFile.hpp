#ifndef HTTP_FILE_HPP_INCLUDED
#define HTTP_FILE_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpOptions.hpp"

namespace Http::detail
{
    class HttpFile : public boost::beast::file
    {
    private:
        fs::path m_path;
        std::size_t m_write_buffer_size;
        boost::beast::flat_buffer m_write_buffer;

    public:
        HttpFile()
            : boost::beast::file{}
            , m_write_buffer_size{ Options::DefaultBufferSize }
            , m_write_buffer{ m_write_buffer_size }
        {}

        virtual ~HttpFile() = default;

        HttpFile(const HttpFile& other) = delete;
        HttpFile& operator=(const HttpFile& other) = delete;

        HttpFile(HttpFile&& other) = default;
        HttpFile& operator=(HttpFile&& other) = default;

        void buffer_size(std::size_t size)
        {
            m_write_buffer_size = size;
            m_write_buffer.reserve(size);
        }

        bool is_open() const
        {
            return boost::beast::file::is_open();
        }

        void close(boost::beast::error_code& ec)
        {
            boost::beast::file::close(ec);
        }

        void close()
        {
            boost::beast::error_code ignored;
            close(ignored);
        }

        void open(char const* path, boost::beast::file_mode mode, boost::beast::error_code& ec)
        {
            m_path = path;
            boost::beast::file::open(path, mode, ec);
        }

        uint64_t size(boost::beast::error_code& ec) const
        {
            return boost::beast::file::size(ec);
        }

        uint64_t size() const
        {
            boost::beast::error_code ec;
            uint64_t result = size(ec);
            if (ec)
            {
                throw boost::beast::system_error(ec);
            }
            return result;
        }

        void remove(std::error_code& ec)
        {
            close();
            std::filesystem::remove(m_path, ec);
        }

        std::size_t pos(boost::beast::error_code& ec)
        {
            return boost::beast::file::pos(ec);
        }

        void seek(std::size_t offset, boost::beast::error_code& ec)
        {
            boost::beast::file::seek(offset, ec);
        }

        std::size_t read(void* buffer, std::size_t n, boost::beast::error_code& ec)
        {
            auto nbRead = boost::beast::file::read(buffer, n, ec);

            if (ec) return nbRead;

            return nbRead;
        }

        std::size_t write(void const* buffer, std::size_t n, boost::beast::error_code& ec)
        {
            if (m_write_buffer.size() + n > m_write_buffer_size)
            {
                flush(ec);
            }

            auto tmpBuffer = m_write_buffer.prepare(n).data();
            std::memcpy(tmpBuffer, buffer, n);
            m_write_buffer.commit(n);

            return n;
        }

        void finalize(boost::beast::error_code& ec)
        {
            flush(ec);
        }

    private:
        void flush(boost::beast::error_code& ec)
        {
            auto writableBuffer = m_write_buffer.data();
            auto nbWritten = boost::beast::file::write(writableBuffer.data(), writableBuffer.size(), ec);
            m_write_buffer.consume(nbWritten);
        }
    };
}

#endif

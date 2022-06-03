#ifndef HTTP_FORM_DATA_HPP_INCLUDED
#define HTTP_FORM_DATA_HPP_INCLUDED

#include "Http/detail/basic_multipart_body.hpp"
#include "Http/HttpMimeTypes.hpp"

namespace Http
{
    class FormData
    {
    private:
        friend class detail::basic_multipart_body<FormData>::reader;
        friend class detail::basic_multipart_body<FormData>::writer;

        struct FileData
        {
            fs::path m_path;
            unsigned long m_chunkOffset;
            unsigned long m_chunkSize;
            std::string m_partHeader;
        };

        typedef std::vector<FileData> Files;

        std::string m_multipart;
        std::string m_boundary;
        std::string m_parameters;
        Files m_files;
        std::string m_closing;
        unsigned long m_size;

    public:
        FormData()
            : m_boundary{}
            , m_parameters{}
            , m_files{}
            , m_closing{}
            , m_size{ 0 }
        {
            auto id = boost::uuids::to_string(boost::uuids::random_generator()());
            boost::erase_all(id, "-");
            m_boundary = std::string(26, '-') + id;
            m_closing = fmt::format("\r\n--{}--\r\n", m_boundary);
            m_multipart = fmt::format("boundary={}", m_boundary);
            m_size += m_closing.size();
        }

        virtual ~FormData() = default;
        FormData(const FormData &other) = default;
        FormData &operator=(const FormData &other) = default;
        FormData(FormData &&other) = default;
        FormData &operator=(FormData &&other) = default;

        FormData &add(const std::string &key, const std::string &value)
        {
            auto parameter = fmt::format(
                "\r\n--{0}\r\n"
                "Content-Disposition: form-data; name=\"{1}\"\r\n\r\n"
                "{2}",
                m_boundary,
                key,
                value);

            m_parameters += parameter;
            m_size += parameter.size();
            return *this;
        }

        FormData &add(const std::string &key, const std::string &name, const fs::path &path)
        {
            if (true == std::filesystem::exists(path))
            {
                return add(key, name, path, 0, std::filesystem::file_size(path));
            }
            else
            {
                throw std::invalid_argument{ "unknown file" };
            }
        }

        FormData &add(const std::string &key, const std::string &name, const fs::path &path, unsigned long chunkOffset, unsigned long chunkSize)
        {
            if (std::filesystem::exists(path))
            {
                unsigned long fileSize = std::filesystem::file_size(path);
                if (chunkOffset <= fileSize)
                {
                    unsigned long remainingSize = fileSize - chunkOffset;
                    unsigned long fixedChunkSize = std::min(remainingSize, chunkSize);
                    std::string partHeader = fmt::format(
                        "\r\n--{0}\r\n"
                        "Content-Disposition: form-data; name=\"{1}\"; filename=\"{2}\"\r\n"
                        "Content-Type: {3}\r\n\r\n",
                        m_boundary,
                        key,
                        name,
                        mime_type(path));

                    m_files.push_back({ path, chunkOffset, fixedChunkSize, partHeader });
                    m_size += partHeader.size() + fixedChunkSize;
                }
                else
                {
                    throw std::invalid_argument{ "invalid offset" };
                }
            }
            else
            {
                throw std::invalid_argument{ "unknown file" };
            }

            return *this;
        }

        const std::string &boundary() const { return m_boundary; }
        unsigned long size() const { return m_size; }
    };
}

#endif

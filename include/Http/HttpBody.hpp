#ifndef HTTP_BODY_HPP_INCLUDED
#define HTTP_BODY_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpFormData.hpp"

namespace Http
{
    using TContent = std::variant<TEmpty, std::string, nlohmann::json, fs::path, FormData>;

    class Body
    {
    private:
        friend class Request;
        friend class Response;
        template <typename DerivedType>
        friend class detail::SessionBase;

        TContent m_content;

    public:
        Body() = default;
        Body(const TEmpty&) : m_content{} {}
        Body(const std::string& text) : m_content{ text } {}
        Body(const nlohmann::json& json) : m_content(json) {}
        Body(const fs::path& path) : m_content{ path } {}
        Body(const FormData& form) : m_content{ form } {}
        template <typename ContentType>
        Body(const ContentType& content) : m_content(nlohmann::json(content)) {}

        virtual ~Body() = default;
        Body(const Body &other) = default;
        Body &operator=(const Body &other) = default;
        Body(Body &&other) = default;
        Body &operator=(Body &&other) = default;

        Body &operator=(const TEmpty&) { m_content = TEmpty{}; return *this; }
        Body &operator=(const std::string& text) { m_content = text; return *this; }
        Body &operator=(const nlohmann::json& json) { m_content = json; return *this; }
        Body &operator=(const fs::path& path) { m_content = path; return *this; }
        Body &operator=(const FormData& form) { m_content = form; return *this; }
        template <typename ContentType>
        Body &operator=(const ContentType& content) { m_content = nlohmann::json(content); return *this; }

        const TContent &content() const { return m_content; }
        const std::string &text() const { return std::get<std::string>(m_content); }
        const nlohmann::json &json() const { return std::get<nlohmann::json>(m_content); }
        const fs::path &path() const { return std::get<fs::path>(m_content); }
        const FormData &form() const { return std::get<FormData>(m_content); }
        template <typename ContentType>
        ContentType get() const { return json().get<ContentType>(); }

        constexpr bool isEmpty() const
        {
            return std::holds_alternative<TEmpty>(m_content);
        }

        constexpr bool isText() const
        {
            return std::holds_alternative<std::string>(m_content);
        }

        constexpr bool isJson() const
        {
            return std::holds_alternative<nlohmann::json>(m_content);
        }

        constexpr bool isPath() const
        {
            return std::holds_alternative<fs::path>(m_content);
        }

        constexpr bool isFormData() const
        {
            return std::holds_alternative<FormData>(m_content);
        }

        bool save(const fs::path &path) const
        {
            auto visitor = overloaded{
                [&path](const TEmpty&){
                    // Nothing to save
                    return true;
                },
                [this, &path](const std::string &arg) {
                    return saveData(arg, path);
                },
                [this, &path](const nlohmann::json &arg) {
                    return saveData(arg.dump(4), path);
                },
                [&path](const fs::path &arg) {
                    if (path.has_parent_path())
                    {
                        std::error_code ec;
                        std::filesystem::create_directories(path.parent_path(), ec);
                        if (ec) return false;
                    }

                    std::error_code ec;
                    std::filesystem::rename(arg, path, ec);
                    if (ec) return false;
                    return true;
                },
                [&path](const FormData&) {
                    //TODO
                    return false;
                }
            };

            return std::visit(visitor, m_content);
        }

    private:
        bool saveData(const std::string& data, const fs::path& path) const
        {
            if (path.has_parent_path())
            {
                std::error_code ec;
                std::filesystem::create_directories(path.parent_path(), ec);
                if (ec) return false;
            }

            std::ofstream file{ path.string() };
            if (false == file.is_open()) return false;

            file << data;
            file.close();
            return true;
        }
    };
}

#endif

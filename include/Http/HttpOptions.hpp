#ifndef HTTP_OPTIONS_HPP_INCLUDED
#define HTTP_OPTIONS_HPP_INCLUDED

#include "Http/HttpForwards.hpp"
#include "Http/HttpAuth.hpp"

namespace Http
{
    using TProgressCb = std::function<void(std::size_t, std::size_t)>;

    class Options
    {
    private:
        std::optional<fs::path> m_ca = {};
        std::optional<fs::path> m_tempDir = {};
        std::optional<fs::path> m_fileOut = {};
        std::optional<bool> m_fileAppend = {};
        std::optional<unsigned long> m_bodyLimit = {};
        std::optional<std::string> m_proxy = {};
        std::optional<std::string> m_auth = {};
        std::optional<TProgressCb> m_sendProgress = {};
        std::optional<double> m_sendStep = {};
        std::optional<TProgressCb> m_recvProgress = {};
        std::optional<double> m_recvStep = {};
        std::optional<unsigned int> m_connectionTimeout = {};
        std::optional<unsigned int> m_requestTimeout = {};
        std::optional<unsigned int> m_nbThreads = {};
        std::optional<std::size_t> m_readBufferSize = {};
        std::optional<std::size_t> m_writeBufferSize = {};

    public:
        static constexpr double DefaultProgressStep = 0.01;
        static constexpr unsigned int DefaultNbThreads = 1;
        static constexpr std::size_t DefaultBufferSize = 64_KiB;

    public:
        inline const std::optional<fs::path>& ca() const { return m_ca; }
        inline Options& ca(const fs::path& ca)
        {
            m_ca = ca;
            return *this;
        }

        inline const std::optional<fs::path>& tempDir() const { return m_tempDir; }
        inline Options& tempDir(const fs::path& tempDir)
        {
            m_tempDir = tempDir;
            return *this;
        }

        inline const std::optional<fs::path>& fileOut() const { return m_fileOut; }
        inline Options& fileOut(const fs::path& fileOut)
        {
            m_fileOut = fileOut;
            return *this;
        }

        inline const std::optional<bool>& fileAppend() const { return m_fileAppend; }
        inline Options& fileAppend(const bool& fileAppend)
        {
            m_fileAppend = fileAppend;
            return *this;
        }

        inline const std::optional<unsigned long>& bodyLimit() const { return m_bodyLimit; }
        inline Options& bodyLimit(const unsigned long bodyLimit)
        {
            m_bodyLimit = bodyLimit;
            return *this;
        }

        inline const std::optional<std::string>& proxy() const { return m_proxy; }
        inline Options& proxy(const std::string& proxy)
        {
            m_proxy = proxy;
            return *this;
        }

        inline const std::optional<std::string>& auth() const { return m_auth; }
        inline Options& auth(const std::string& auth)
        {
            m_auth = auth;
            return *this;
        }

        template <AuthType authType>
        inline Options& auth(const Auth<authType>& auth)
        {
            m_auth = auth.value;
            return *this;
        }

        inline const std::optional<TProgressCb>& sendProgress() const { return m_sendProgress; }
        inline const std::optional<double>& sendStep() const { return m_sendStep; }
        inline Options& sendProgress(const TProgressCb& sendProgress)
        {
            m_sendProgress = sendProgress;
            m_sendStep = DefaultProgressStep;
            return *this;
        }
        inline Options& sendProgress(const TProgressCb& sendProgress, double sendStep)
        {
            m_sendProgress = sendProgress;
            m_sendStep = sendStep;
            return *this;
        }
        inline Options& sendStep(double sendStep)
        {
            m_sendStep = sendStep;
            return *this;
        }

        inline const std::optional<TProgressCb>& recvProgress() const { return m_recvProgress; }
        inline const std::optional<double>& recvStep() const { return m_recvStep; }
        inline Options& recvProgress(const TProgressCb& recvProgress)
        {
            m_recvProgress = recvProgress;
            m_recvStep = DefaultProgressStep;
            return *this;
        }
        inline Options& recvProgress(const TProgressCb& recvProgress, double recvStep)
        {
            m_recvProgress = recvProgress;
            m_recvStep = recvStep;
            return *this;
        }
        inline Options& recvStep(double recvStep)
        {
            m_recvStep = recvStep;
            return *this;
        }

        inline const std::optional<unsigned int>& connectionTimeout() const { return m_connectionTimeout; }
        inline Options& connectionTimeout(const unsigned int connectionTimeout)
        {
            m_connectionTimeout = connectionTimeout;
            return *this;
        }

        inline const std::optional<unsigned int>& requestTimeout() const { return m_requestTimeout; }
        inline Options& requestTimeout(const unsigned int requestTimeout)
        {
            m_requestTimeout = requestTimeout;
            return *this;
        }

        inline const std::optional<unsigned int> nbThreads() const { return m_nbThreads; }
        inline Options& nbThreads(unsigned int nbThreads)
        {
            m_nbThreads = nbThreads;
            return *this;
        }

        inline const std::optional<std::size_t>& readBufferSize() const { return m_readBufferSize; }
        inline Options& readBufferSize(std::size_t bufferSize)
        {
            m_readBufferSize = bufferSize;
            return *this;
        }

        inline const std::optional<std::size_t>& writeBufferSize() const { return m_writeBufferSize; }
        inline Options& writeBufferSize(std::size_t bufferSize)
        {
            m_writeBufferSize = bufferSize;
            return *this;
        }

        inline Options operator+(const Options& other) const
        {
            Options result = *this;
            if (other.ca().has_value()) result.ca(other.ca().value());
            if (other.tempDir().has_value()) result.tempDir(other.tempDir().value());
            if (other.fileOut().has_value()) result.fileOut(other.fileOut().value());
            if (other.fileAppend().has_value()) result.fileAppend(other.fileAppend().value());
            if (other.bodyLimit().has_value()) result.bodyLimit(other.bodyLimit().value());
            if (other.proxy().has_value()) result.proxy(other.proxy().value());
            if (other.auth().has_value()) result.auth(other.auth().value());
            if (other.sendProgress().has_value()) result.sendProgress(other.sendProgress().value());
            if (other.sendStep().has_value()) result.sendStep(other.sendStep().value());
            if (other.recvProgress().has_value()) result.recvProgress(other.recvProgress().value());
            if (other.recvStep().has_value()) result.recvStep(other.recvStep().value());
            if (other.connectionTimeout().has_value()) result.connectionTimeout(other.connectionTimeout().value());
            if (other.requestTimeout().has_value()) result.requestTimeout(other.requestTimeout().value());
            if (other.nbThreads().has_value()) result.nbThreads(other.nbThreads().value());
            if (other.readBufferSize().has_value()) result.readBufferSize(other.readBufferSize().value());
            if (other.writeBufferSize().has_value()) result.writeBufferSize(other.writeBufferSize().value());

            return result;
        }
    };
}

#endif

#ifndef HTTP_MIME_TYPES_HPP_INCLUDED
#define HTTP_MIME_TYPES_HPP_INCLUDED

#include "Http/HttpForwards.hpp"

namespace Http
{
    inline std::string mime_type(const fs::path& path)
    {
        auto const ext = path.extension().string();

        if (boost::iequals(ext, ".aac"))    return "audio/aac";
        if (boost::iequals(ext, ".abw"))    return "application/x-abiword";
        if (boost::iequals(ext, ".arc"))    return "application/x-freearc";
        if (boost::iequals(ext, ".avi"))    return "video/x-msvideo";
        if (boost::iequals(ext, ".azw"))    return "application/vnd.amazon.ebook";
        if (boost::iequals(ext, ".bin"))    return "application/octet-stream";
        if (boost::iequals(ext, ".bmp"))    return "image/bmp";
        if (boost::iequals(ext, ".bz"))     return "application/x-bzip";
        if (boost::iequals(ext, ".bz2"))    return "application/x-bzip2";
        if (boost::iequals(ext, ".csh"))    return "application/x-csh";
        if (boost::iequals(ext, ".css"))    return "text/css";
        if (boost::iequals(ext, ".csv"))    return "text/csv";
        if (boost::iequals(ext, ".doc"))    return "application/msword";
        if (boost::iequals(ext, ".docx"))   return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        if (boost::iequals(ext, ".eot"))    return "application/vnd.ms-fontobject";
        if (boost::iequals(ext, ".epub"))   return "application/epub+zip";
        if (boost::iequals(ext, ".gz"))     return "application/gzip";
        if (boost::iequals(ext, ".gif"))    return "image/gif";
        if (boost::iequals(ext, ".htm"))    return "text/html";
        if (boost::iequals(ext, ".html"))   return "text/html";
        if (boost::iequals(ext, ".ico"))    return "image/vnd.microsoft.icon";
        if (boost::iequals(ext, ".ics"))    return "text/calendar";
        if (boost::iequals(ext, ".jar"))    return "application/java-archive";
        if (boost::iequals(ext, ".jpeg"))   return "image/jpeg";
        if (boost::iequals(ext, ".jpg"))    return "image/jpeg";
        if (boost::iequals(ext, ".js"))     return "text/javascript";
        if (boost::iequals(ext, ".json"))   return "application/json";
        if (boost::iequals(ext, ".jsonld")) return "application/ld+json";
        if (boost::iequals(ext, ".mid"))    return "audio/midi audio/x-midi";
        if (boost::iequals(ext, ".midi"))   return "audio/midi audio/x-midi";
        if (boost::iequals(ext, ".mjs"))    return "text/javascript";
        if (boost::iequals(ext, ".mp3"))    return "audio/mpeg";
        if (boost::iequals(ext, ".mpeg"))   return "video/mpeg";
        if (boost::iequals(ext, ".mpkg"))   return "application/vnd.apple.installer+xml";
        if (boost::iequals(ext, ".odp"))    return "application/vnd.oasis.opendocument.presentation";
        if (boost::iequals(ext, ".ods"))    return "application/vnd.oasis.opendocument.spreadsheet";
        if (boost::iequals(ext, ".odt"))    return "application/vnd.oasis.opendocument.text";
        if (boost::iequals(ext, ".oga"))    return "audio/ogg";
        if (boost::iequals(ext, ".ogv"))    return "video/ogg";
        if (boost::iequals(ext, ".ogx"))    return "application/ogg";
        if (boost::iequals(ext, ".opus"))   return "audio/opus";
        if (boost::iequals(ext, ".otf"))    return "font/otf";
        if (boost::iequals(ext, ".png"))    return "image/png";
        if (boost::iequals(ext, ".pdf"))    return "application/pdf";
        if (boost::iequals(ext, ".php"))    return "application/x-httpd-php";
        if (boost::iequals(ext, ".ppt"))    return "application/vnd.ms-powerpoint";
        if (boost::iequals(ext, ".pptx"))   return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        if (boost::iequals(ext, ".rar"))    return "application/vnd.rar";
        if (boost::iequals(ext, ".rtf"))    return "application/rtf";
        if (boost::iequals(ext, ".sh"))     return "application/x-sh";
        if (boost::iequals(ext, ".svg"))    return "image/svg+xml";
        if (boost::iequals(ext, ".swf"))    return "application/x-shockwave-flash";
        if (boost::iequals(ext, ".tar"))    return "application/x-tar";
        if (boost::iequals(ext, ".tif"))    return "image/tiff";
        if (boost::iequals(ext, ".tiff"))   return "image/tiff";
        if (boost::iequals(ext, ".ts"))     return "video/mp2t";
        if (boost::iequals(ext, ".ttf"))    return "font/ttf";
        if (boost::iequals(ext, ".txt"))    return "text/plain";
        if (boost::iequals(ext, ".vsd"))    return "application/vnd.visio";
        if (boost::iequals(ext, ".wav"))    return "audio/wav";
        if (boost::iequals(ext, ".weba"))   return "audio/webm";
        if (boost::iequals(ext, ".webm"))   return "video/webm";
        if (boost::iequals(ext, ".webp"))   return "image/webp";
        if (boost::iequals(ext, ".woff"))   return "font/woff";
        if (boost::iequals(ext, ".woff2"))  return "font/woff2";
        if (boost::iequals(ext, ".xhtml"))  return "application/xhtml+xml";
        if (boost::iequals(ext, ".xls"))    return "application/vnd.ms-excel";
        if (boost::iequals(ext, ".xlsx"))   return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        if (boost::iequals(ext, ".xml"))    return "application/xml";
        if (boost::iequals(ext, ".xul"))    return "application/vnd.mozilla.xul+xml";
        if (boost::iequals(ext, ".zip"))    return "application/zip";
        if (boost::iequals(ext, ".3gp"))    return "video/3gpp";
        if (boost::iequals(ext, ".3g2"))    return "video/3gpp2";
        if (boost::iequals(ext, ".7z"))     return "application/x-7z-compressed";

        return "application/octet-stream";
    }
}

#endif

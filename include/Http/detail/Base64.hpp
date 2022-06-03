/*
   Modified version of base64.cpp and base64.h from:
   https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/

   Original disclaimer:
*/
/*
   base64.cpp and base64.h
   
   base64 encoding and decoding with C++.
   More information at
     https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp

   Version: 2.rc.08 (release candidate)

   Copyright (C) 2004-2017, 2020, 2021 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#ifndef BASE64_HPP_INCLUDED
#define BASE64_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>

namespace
{
    // Depending on the url parameter in base64_chars, one of two sets of base64 characters needs to be chosen.
    // They differ in their last two characters.
    static constexpr char const *base64_chars[2] =
    {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/",

        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "-_" };

    inline unsigned int charPos(const unsigned char chr)
    {
        if (chr >= 'A' && chr <= 'Z')
            return chr - 'A';
        else if (chr >= 'a' && chr <= 'z')
            return chr - 'a' + ('Z' - 'A') + 1;
        else if (chr >= '0' && chr <= '9')
            return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
        else if (chr == '+' || chr == '-')
            return 62;
        else if (chr == '/' || chr == '_')
            return 63;
        else
            throw std::invalid_argument("Input is not valid base64-encoded data.");
    }
}

namespace Http::detail::Base64
{
    inline std::string encode(const std::string &s, bool url = false)
    {
        const unsigned char *bytes_to_encode = reinterpret_cast<const unsigned char *>(s.data());
        std::size_t length = s.length();
        std::size_t len_encoded = (length + 2) / 3 * 4;

        unsigned char trailing_char = url ? '.' : '=';

        const char *base64_chars_ = base64_chars[url];

        std::string result;
        result.reserve(len_encoded);

        unsigned int pos = 0;

        while (pos < length)
        {
            result.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

            if (pos + 1 < length)
            {
                result.push_back(base64_chars_[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

                if (pos + 2 < length)
                {
                    result.push_back(base64_chars_[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
                    result.push_back(base64_chars_[bytes_to_encode[pos + 2] & 0x3f]);
                }
                else
                {
                    result.push_back(base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
                    result.push_back(trailing_char);
                }
            }
            else
            {
                result.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
                result.push_back(trailing_char);
                result.push_back(trailing_char);
            }

            pos += 3;
        }

        return result;
    }

    inline std::string decode(const std::string &s, bool removeLineBreaks)
    {
        if (s.empty())
            return std::string{};

        if (removeLineBreaks)
        {
            std::string copy{ s };

            copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

            return decode(copy, false);
        }

        std::size_t length = s.length();
        std::size_t pos = 0;

        std::size_t approxLength = length / 4 * 3;
        std::string ret;
        ret.reserve(approxLength);

        while (pos < length)
        {
            unsigned int pos1 = charPos(s[pos + 1]);

            ret.push_back(static_cast<std::string::value_type>(((charPos(s[pos + 0])) << 2) + ((pos1 & 0x30) >> 4)));

            if ((pos + 2 < length) &&
                s[pos + 2] != '=' &&
                s[pos + 2] != '.')
            {
                unsigned int pos2 = charPos(s[pos + 2]);
                ret.push_back(static_cast<std::string::value_type>(((pos1 & 0x0f) << 4) + ((pos2 & 0x3c) >> 2)));

                if ((pos + 3 < length) &&
                    s[pos + 3] != '=' &&
                    s[pos + 3] != '.')
                {
                    ret.push_back(static_cast<std::string::value_type>(((pos2 & 0x03) << 6) + charPos(s[pos + 3])));
                }
            }

            pos += 4;
        }

        return ret;
    }
}

#endif

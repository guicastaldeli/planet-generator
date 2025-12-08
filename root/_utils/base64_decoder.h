#pragma once
#include <string>
#include <vector>

class Base64Decoder {
    private:
        static const std::string chars;
        static inline bool isBase64(unsigned char c);

    public:
        static std::vector<unsigned char> decode(const std::string& encoded);
        static bool isValid(const std::string& str);
};
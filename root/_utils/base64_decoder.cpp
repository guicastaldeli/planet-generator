#include "base64_decoder.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

const std::string Base64Decoder::chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

bool Base64Decoder::isBase64(unsigned char c) {
    return (isalnum(c) ||
        (c == '+') ||
        (c == '/')
    );
}

bool Base64Decoder::isValid(const std::string& str) {
    if(str.length() % 4 != 0) return false;
    for(char c : str) {
        if(!isBase64(static_cast<unsigned char>(c)) && c != '=') {
            return false;
        }
    }
    return true;
}

std::vector<unsigned char> Base64Decoder::decode(const std::string& encoded) {
    std::string clean_encoded = encoded;
    
    size_t pos = clean_encoded.find("base64,");
    if(pos != std::string::npos) {
        clean_encoded = clean_encoded.substr(pos + 7);
    }
    
    clean_encoded.erase(std::remove_if(clean_encoded.begin(), clean_encoded.end(), 
        [](char c) { return std::isspace(c); }), clean_encoded.end());
    
    if(!isValid(clean_encoded)) {
        throw std::runtime_error("Invalid base64 string");
    }
    
    int in_len = clean_encoded.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> ret;
    
    while(in_len-- && (clean_encoded[in_] != '=') && isBase64(clean_encoded[in_])) {
        char_array_4[i++] = clean_encoded[in_]; in_++;
        if(i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }
    
    if(i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j < 4; j++)
            char_array_4[j] = chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++)
            ret.push_back(char_array_3[j]);
    }
    
    return ret;
}
#include "base64_decoder.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <iostream>

const std::string Base64Decoder::chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

bool Base64Decoder::isBase64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::vector<unsigned char> Base64Decoder::decode(const std::string& encoded_string) {
    std::string encoded = encoded_string;
    
    size_t base64_start = encoded.find("base64,");
    if(base64_start != std::string::npos) {
        encoded = encoded.substr(base64_start + 7);
    }
    
    encoded.erase(std::remove_if(encoded.begin(), encoded.end(), 
                   [](unsigned char c) { return std::isspace(c); }), 
                   encoded.end());
    
    int in_len = encoded.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> ret;
    
    for(int k = 0; k < std::min(20, in_len); k++) {
        std::cout << encoded[k];
    }
    std::cout << std::endl;
    
    while(in_len-- && encoded[in_] != '=' && isBase64(encoded[in_])) {
        char_array_4[i++] = encoded[in_]; 
        in_++;
        
        if(i == 4) {
            for(i = 0; i < 4; i++) {
                size_t pos = chars.find(char_array_4[i]);
                if(pos == std::string::npos) {
                    std::cerr << "Invalid base64 character: " << char_array_4[i] << std::endl;
                    throw std::runtime_error("Invalid base64 character");
                }
                char_array_4[i] = static_cast<unsigned char>(pos);
            }
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0x0F) << 4) + ((char_array_4[2] & 0x3C) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x03) << 6) + char_array_4[3];
            
            for(i = 0; i < 3; i++) {
                ret.push_back(char_array_3[i]);
            }
            i = 0;
        }
    }
    
    std::cout << "Decoded " << ret.size() << " bytes so far" << std::endl;
    
    if(i) {
        for(j = i; j < 4; j++) {
            char_array_4[j] = 0;
        }
        for(j = 0; j < 4; j++) {
            if(in_ < encoded.size() && encoded[in_] == '=') {
                char_array_4[j] = 0;
                in_++;
            } else if(in_ < encoded.size()) {
                size_t pos = chars.find(encoded[in_]);
                if(pos == std::string::npos) {
                    std::cerr << "Invalid character in padding: " << encoded[in_] << std::endl;
                    throw std::runtime_error("Invalid base64 character");
                }
                char_array_4[j] = static_cast<unsigned char>(pos);
                in_++;
            }
        }
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0x0F) << 4) + ((char_array_4[2] & 0x3C) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x03) << 6) + char_array_4[3];
        
        for(j = 0; j < i - 1; j++) {
            ret.push_back(char_array_3[j]);
        }
    }
    
    std::cout << "Total decoded bytes: " << ret.size() << std::endl;
    if(ret.empty()) {
        std::cerr << "WARNING: Decoded data is empty!" << std::endl;
    } else {
        std::cout << "First few decoded bytes (hex): ";
        for(int k = 0; k < std::min(10, (int)ret.size()); k++) {
            printf("%02x ", ret[k]);
        }
        std::cout << std::endl;
    }
    
    return ret;
}
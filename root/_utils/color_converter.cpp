#include "color_converter.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <cmath>
#include <iostream>

ColorConverter::ColorConverter() {}
ColorConverter::~ColorConverter() {}

/*
 * Hex to Rgb
 */
glm::vec3 ColorConverter::hexToRgb(const std::string& hex) {
    if(hex.empty()) return glm::vec3(0.5f, 0.5f, 0.5f);
    
    std::string cleanHex = hex;
    if(cleanHex[0] == '#') cleanHex = cleanHex.substr(1);
    if(cleanHex.length() == 3) {
        std::string fullHex;
        for(char c : cleanHex) {
            fullHex += c;
            fullHex += c;
        }
        cleanHex = fullHex;
    }
    if(cleanHex.length() == 8) {
        cleanHex = cleanHex.substr(0, 6);
    }
    if(cleanHex.length() != 6) {
        return glm::vec3(0.5f, 0.5f, 0.5f);
    }
    
    unsigned int r;
    unsigned int g;
    unsigned int b;
    std::stringstream str;
    str << std::hex << cleanHex.substr(0, 2);
    str >> r;
    str.clear();
    str << std::hex << cleanHex.substr(2, 2);
    str >> g;
    str.clear();
    str << std::hex << cleanHex.substr(4, 2);
    str >> b;
    
    return glm::vec3(
        r / 255.0f, 
        g / 255.0f, 
        b / 255.0f
    );
}


/*
 * Rgb to Hex
 */
std::string ColorConverter::rgbToHex(const glm::vec3& rgb) {
    float r = std::max(0.0f, std::min(1.0f, rgb.r));
    float g = std::max(0.0f, std::min(1.0f, rgb.g));
    float b = std::max(0.0f, std::min(1.0f, rgb.b));
    
    int ri = static_cast<int>(r * 255.0f);
    int gi = static_cast<int>(g * 255.0f);
    int bi = static_cast<int>(b * 255.0f);
    
    std::stringstream str;
    str << "#" 
       << std::hex << std::setw(2) << std::setfill('0') << ri
       << std::hex << std::setw(2) << std::setfill('0') << gi
       << std::hex << std::setw(2) << std::setfill('0') << bi;
    return str.str();
}

/*
 * Parse Rgb String
 */
glm::vec3 ColorConverter::parseRgbString(const std::string& rgbStr) {
    if(rgbStr.empty()) return glm::vec3(0.5f, 0.5f, 0.5f);
    
    std::string input = rgbStr;
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
    
    if(input.find("rgb(") == 0 && input[input.length()-1] == ')') {
        std::string content = input.substr(4, input.length() - 5);
        std::vector<float> values;
        
        std::stringstream str(content);
        std::string token;
        
        while(std::getline(str, token, ',')) {
            try {
                float val = std::stof(token);
                values.push_back(val);
            } catch(...) {
                std::cerr << "input err" << std::endl;
            }
        }
        
        if(values.size() >= 3) {
            return glm::vec3(
                values[0] / 255.0f, 
                values[1] / 255.0f, 
                values[2] / 255.0f
            );
        }
    }
    
    return hexToRgb(rgbStr);
}

/*
 * Parse Color
 */
glm::vec3 ColorConverter::parseColor(const std::string& color) {
    if(color.empty()) return glm::vec3(0.5f, 0.5f, 0.5f);
    if(color[0] == '#') return hexToRgb(color);
    
    std::string lower = color;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if(lower.find("rgb") == 0) return parseRgbString(color);

    static const std::unordered_map<std::string, glm::vec3> namedColors = {
        { "red", glm::vec3(1.0f, 0.0f, 0.0f) },
        { "green", glm::vec3(0.0f, 1.0f, 0.0f) },
        { "blue", glm::vec3(0.0f, 0.0f, 1.0f) },
        { "white", glm::vec3(1.0f, 1.0f, 1.0f) },
        { "black", glm::vec3(0.0f, 0.0f, 0.0f) },
        { "gray", glm::vec3(0.5f, 0.5f, 0.5f) },
        { "yellow", glm::vec3(1.0f, 1.0f, 0.0f) },
        { "cyan", glm::vec3(0.0f, 1.0f, 1.0f) },
        { "magenta", glm::vec3(1.0f, 0.0f, 1.0f) },
        { "orange", glm::vec3(1.0f, 0.5f, 0.0f) },
        { "purple", glm::vec3(0.5f, 0.0f, 0.5f) }
    };
    auto it = namedColors.find(lower);
    if(it != namedColors.end()) {
        return it->second;
    }
    
    return glm::vec3(0.5f, 0.5f, 0.5f);
}

std::string ColorConverter::formatRgb(const glm::vec3& rgb) {
    float r = std::max(0.0f, std::min(1.0f, rgb.r));
    float g = std::max(0.0f, std::min(1.0f, rgb.g));
    float b = std::max(0.0f, std::min(1.0f, rgb.b));
    
    int ri = static_cast<int>(std::round(r * 255.0f));
    int gi = static_cast<int>(std::round(g * 255.0f));
    int bi = static_cast<int>(std::round(b * 255.0f));
    
    std::stringstream str;
    str << "rgb(" << ri << ", " << gi << ", " << bi << ")";
    return str.str();
}
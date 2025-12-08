#pragma once
#include <string>
#include <glm/glm.hpp>

class ColorConverter {
    public:
        ColorConverter();
        ~ColorConverter();
        
        static glm::vec3 hexToRgb(const std::string& hex);
        static std::string rgbToHex(const glm::vec3& rgb);
        static std::string formatRgb(const glm::vec3& rgb);

        static glm::vec3 parseColor(const std::string& color);
        static glm::vec3 parseRgbString(const std::string& rgbStr);
};      
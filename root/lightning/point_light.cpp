#include "point_light.h"

PointLight::PointLight() :
    position(0.0f, 0.0f, 0.0f),
    color(1.0f, 1.0f, 1.0f),
    intensity(1.0f),
    constant(1.0f),
    linear(0.09f),
    quadratic(0.032f)
{
    calcRadius();
};
PointLight::~PointLight() {};

void PointLight::calcRadius() {
    const float threshold = 0.01f;
    float maxBrightness = intensity * glm::max(glm::max(color.r, color.g), color.b);
    radius = 
        (-linear + sqrt(linear * linear - 4.0f * quadratic * (constant - maxBrightness / threshold))) / 
        (2.0f * quadratic);
}

/**
 * Add
 */
void PointLight::add(const PointLight& pointLight) {
    if(pointLights.size() < 15) {
        pointLights.push_back(pointLight);
    }
}

/**
 * Remove
 */
void PointLight::remove(int i) {
    if(i >= 0 && i < pointLights.size()) {
        pointLights.erase(pointLights.begin() + i);
    }
}

/**
 * Clear
 */
void PointLight::clear() {
    pointLights.clear();
}

/**
 * Get
 */
const std::vector<PointLight>& PointLight::get() const {
    return pointLights;
}
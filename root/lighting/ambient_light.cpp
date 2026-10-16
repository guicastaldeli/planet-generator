#include "ambient_light.h"
#include <glm/glm.hpp>

AmbientLight::AmbientLight() :
    color(glm::vec3(0.1333f, 0.1333f, 0.1333f)),
    intensity(1.0f),
    enabled(true)
{};
AmbientLight::~AmbientLight() {};

/**
 * Color
 */
void AmbientLight::setColor(const glm::vec3& color) {
    this->color = color;
}

glm::vec3 AmbientLight::getColor() const {
    return color;
}

/**
 * Intensity
 */
void AmbientLight::setIntensity(float intensity) {
    this->intensity = intensity;
}

float AmbientLight::getIntensity() const {
    return intensity;
}

/**
 * Enabled
 */
void AmbientLight::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool AmbientLight::isEnabled() const {
    return enabled;
}
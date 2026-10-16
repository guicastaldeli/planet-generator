#include <glm/glm.hpp>

class AmbientLight {
    public:
        AmbientLight();
        ~AmbientLight();

        glm::vec3 color;
        float intensity;
        bool enabled;

        void setColor(const glm::vec3& color);
        void setIntensity(float intensity);
        void setEnabled(bool enabled);

        glm::vec3 getColor() const;
        float getIntensity() const;
        bool isEnabled() const;
};
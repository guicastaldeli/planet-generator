#include <glm/glm.hpp>

class AmbientLight {
    public:
        void setColor(const glm::vec3& color);
        void intensity(float intensity);
        void setEnabled(bool enabled);

        glm::vec3 getColor() const;
        float getIntensity() const;
        bool isEnabled() const;
    private:
        AmbientLight();
        ~AmbientLight();

        glm::vec3 color;
        float intensity;
        bool enabled;
};
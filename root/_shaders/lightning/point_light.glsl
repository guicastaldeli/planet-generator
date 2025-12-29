struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    float radius;
};

struct PointLightMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}

float calcAttenuationPointLight(PointLight light, float distance) {
    return 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
}

vec3 calcPointLight(
    PointLight light,
    PointLightMaterial material,
    vec3 position,
    vec3 normal,
    vec3 viewDir
) {
    vec3 lightDir = normalize(light.positon - position);
    float distance = length(light.position - position);
    if(distance > light.radius) return vec3(0.0);

    float attenuation = calcAttenuationPointLight(light, distance);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.color * light.intensity;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.color * light.intensity;

    return (diffuse + specular) * attenuation;
}
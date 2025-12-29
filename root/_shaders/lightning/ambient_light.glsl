struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

vec3 calcAmbient(
    Material material, 
    vec3 lightColor, 
    float intensity
) {
    return material.ambient * lightColor * intensity;
}

vec3 calcDiffuse(
    Material material,
    vec3 lightColor,
    float intensity,
    vec3 normal,
    vec3 lightDir,
    vec3 viewDir
) {
    float diff = max(dot(normal, lightDir), 0.0);
    return material.diffuse * diff * lightColor * intensity;
}

vec3 calcSpecular(
    Material material,
    vec3 lightColor,
    float intensity,
    vec3 normal,
    vec3 lightDir,
    vec3 viewDir
) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return material.specular * spec * lightColor * intensity;
}

vec3 calcPhongLightning(
    vec3 position,
    vec3 normal,
    vec3 viewDir,
    Light light,
    Material material
) {
    vec3 lightDir = normalize(light.position - position);
    vec3 ambient = calcAmbient(
        material, 
        light.color, 
        light.intensity
    );
    vec3 diffuse = calcDiffuse(
        material, 
        light.color,
        light.intensity, 
        normal, 
        lightDir, 
        viewDir
    );
    vec3 specular = calcSpecular(
        material, 
        light.color,
        light.intensity, 
        normal, 
        lightDir, 
        viewDir
    );

    return ambient + diffuse + specular;
}
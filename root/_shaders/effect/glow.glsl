vec3 applyGlow(
    vec3 baseColor,
    vec3 normal,
    vec3 viewDir,
    float emissiveStrength,
    float time,
    float planetSize
) {
    if(emissiveStrength <= 0.0) {
        return vec3(0.0);
    }

    float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
    fresnel = pow(fresnel, 1.5);
    
    float radialEffect = (normal.y * 0.5 + 0.5) * 0.3;
    
    float glow = (fresnel + radialEffect) * emissiveStrength;
    float pulse = 0.9 + 0.1 * sin(time * 2.0);

    return baseColor * glow * pulse;
}
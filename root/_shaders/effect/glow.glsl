vec3 applyGlow(
    vec3 baseColor,
    vec3 normal,
    vec3 viewDir,
    float emissiveStrength,
    float time,
    float planetSize
) {
    if(emissiveStrength <= 0.0) {
        return baseColor;
    }

    float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
    fresnel = pow(fresnelm 1.5);

    vec2 uv = gl_PointColor - 0.5;
    float dist = length(uv);
    float radialGlow = 1.0 - smoothstep(0.0, 0.5, dist)
}
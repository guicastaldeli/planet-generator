vec3 applyFresnel(vec3 baseColor, vec3 normal, vec3 viewDir) {
    float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
    fresnel = pow(fresnel, 2.0);
    vec3 orbitColor = vec3(0.2, 0.4, 1.0);
    return mix(baseColor, orbitColor, fresnel * 1.2);
}
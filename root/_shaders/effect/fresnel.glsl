vec3 applyFresnel(
    vec3 baseColor, 
    vec3 normal, 
    vec3 viewDir,
    vec3 worldPos,
    float planetSize,
    float uTime
) {
    float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
    fresnel = pow(fresnel, 1.5);

    float distFromCenter = length(worldPos);
    float normalizedDist = distFromCenter / planetSize;

    float glowFalloff = 0.3;
    float glow = exp(-(normalizedDist - 1.0) / glowFalloff);

    float totalEffect = fresnel * (1.0 + glow * 2.0);
    totalEffect = clamp(totalEffect, 0.0, 1.0);

    float pulse = sin(uTime * 0.5) + 0.05 + 0.95;
    vec3 atmosphereCore = vec3(0.15, 0.35, 0.8);
    vec3 atmosphereMid = vec3(0.2588, 0.3843, 0.6824);
    vec3 atmosphereEdge = vec3(0.5176, 0.6157, 0.8471);

    vec3 atmosphereColor;
    if(totalEffect < 0.5) {
        atmosphereColor = mix(
            atmosphereCore,
            atmosphereMid,
            totalEffect * 2.0
        );
    } else {
        atmosphereColor = mix(
            atmosphereMid,
            atmosphereEdge,
            (totalEffect - 0.5) * 2.0
        );
    }

    return mix(baseColor, atmosphereColor, totalEffect * 0.7 * pulse);
}
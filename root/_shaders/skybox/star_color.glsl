vec4 getStarColor(
    vec3 color, 
    float brightness, 
    float time,
    float phase
) {
    float twinkle = 0.7 + 0.3 * sin(time * 5.0 + phase);
    float alpha = brightness * twinkle;

    float dist = length(gl_PointCoord - 0.5);
    float falloff = 1.0 - smoothstep(0.03, 0.12, dist);

    return vec4(color, alpha * falloff);
}
vec4 getStarColor(
    vec3 color, 
    float brightness, 
    float time,
    float phase
) {
    float twinkle = 0.7 + 0.3 * sin(time * 5.0 + phase);
    float alpha = smoothstep(0.0, 1.0, brightness * twinkle);

    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    float dist = length(uv);
    float falloff = 1.0 - smoothstep(0.0, 1.0, dist);

    return vec4(color, alpha * falloff);
}
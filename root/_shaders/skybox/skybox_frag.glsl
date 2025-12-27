uniform float uTime;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec3 genStars(vec3 dir, float time) {
    vec2 uv = vec2(
        atan(dir.x, dir.z) / (2.0 * 3.14159265),
        asin(dir.y) / 3.14159265
    );

    float starDensity = 0.0005;
    float starVal = random(uv * 1000.0);
    if(starVal > 1.0 - starDensity) {
        float brightness = (starVal - (1.0 - starDensity)) / starDensity;
        float twinkle = 0.7 + 0.3 * sin(time * 3.0 + uv.x * 10.0);
        return vec3(1.0) * brightness * twinkle;
    }

    return vec3(0.0);
}

void getSkyboxColor(vec3 dir, float time) {
    vec3 color = vec3(0.0, 0.0, 0.0);
    color += genStars(dir, time);
    return vec4(color, 1.0);
}
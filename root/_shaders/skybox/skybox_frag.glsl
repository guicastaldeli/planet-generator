float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec3 genStars(vec3 dir, float time) {
    float starVal = random(dir.xy * 100.0 + dir.z * 10.0);
    float starDestiny = 0.001;
    if(starVal > 1.0 - starDestiny) {
        float brightness = (starVal - (1.0 - starDestiny)) / starDestiny;
        float twinkle = 0.7 + 0.3 * sin(time * 2.0 + dir.x * 10.0);
        return vec3(1.0, 1.0, 0.9) * brightness * twinkle * 3.0;
    }
    return vec3(0.0);
}

vec4 getSkyboxColor(vec3 dir, float time) {
    //vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 color = vec3(0.5176, 0.2745, 0.2745);
    color += genStars(dir, time);
    return vec4(color, 1.0);
}
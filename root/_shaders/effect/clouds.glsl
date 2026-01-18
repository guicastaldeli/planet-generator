vec4 applyClouds(
    vec3 baseColor, 
    vec3 normal, 
    vec3 position,
    float planetSize,
    float time
) {
    if(uIsClouds < 0.5) {
        return vec4(baseColor, 1.0);
    }

    if(uUseTex) {
        vec2 texCoord = vec2(
            (atan(normal.x, normal.z) / (2.0 * 3.14159265359) + 0.5),
            (asin(normal.y) / 3.14159265359 + 0.5)
        );

        vec4 texColor = texture2D(uTex, texCoord);
        float cloudCoverage = texColor.a;
        vec3 finalColor = mix(baseColor, texColor.rgb, cloudCoverage * 0.8);

        return vec4(finalColor, 1.0);
    } else {
        float noiseVal = sin(position.x * 10.0 + time) * 0.5 + 0.5;
        float clouds = smoothstep(0.3, 0.7, noiseVal);
        vec3 cloudColor = vec3(1.0, 1.0, 1.0);
        vec3 finalColor = mix(baseColor, cloudColor, clouds * 0.6);
        return vec4(finalColor, 1.0);
    }
}
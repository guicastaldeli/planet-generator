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

    vec2 texCoord = vTexCoord;
    
    float cloudSpeed = 0.1;
    texCoord.x += time * cloudSpeed;
    
    vec4 texColor = texture2D(uTex, texCoord);
    float cloudAlpha = texColor.a;
    
    vec3 finalColor = mix(baseColor, texColor.rgb, cloudAlpha);
    return vec4(finalColor, cloudAlpha);
}
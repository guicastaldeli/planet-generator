vec4 applyRings(
    vec3 baseColor, 
    vec3 normal, 
    vec3 position,
    float planetSize,
    float time
) {
    if(uIsRings < 0.5) {
        return vec4(baseColor, 1.0);
    }

    vec2 texCoord = vec2(vTexCoord.x, vTexCoord.y);
    
    float ringSpeed = 0.05;
    texCoord.x += time * ringSpeed;
    
    vec4 texColor = texture2D(uTex, texCoord);
    float ringAlpha = texColor.a;
    
    if(length(texColor.rgb) < 0.1) {
        texColor.rgb = vec3(0.0941, 0.0784, 0.0588);
    }
    
    vec3 ringColor = mix(baseColor, texColor.rgb, ringAlpha);
    return vec4(ringColor, ringAlpha);
}
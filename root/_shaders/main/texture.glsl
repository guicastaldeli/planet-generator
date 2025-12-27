uniform sampler2D uTex;
uniform bool uUseTex;

vec3 getBaseColor() {
    if(uUseTex) {
        vec2 flippedCoord = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
        return texture2D(uTex, flippedCoord).rgb;
    } else {
        return vColor;
    }
}
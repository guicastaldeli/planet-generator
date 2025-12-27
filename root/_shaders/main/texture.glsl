uniform sampler2D uTex;
uniform bool uUseTex;

vec3 getBaseColor() {
    if(uUseTex) {
        return texture2D(uTex, vTexCoord).rgb;
    } else {
        return vColor;
    }
}
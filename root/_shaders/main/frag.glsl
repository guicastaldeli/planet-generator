precision mediump float;

varying vec3 vColor;
uniform float isHovered;

varying vec2 vTexCoord;
uniform sampler2D uTex;
uniform bool uUseTex;

void main() {
    vec3 base;
    if(uUseTex) {
        base = texture2D(uTex, vTexCoord).rgb;
    } else {
        base = vColor;
    }
    vec3 hoverColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(base, hoverColor, isHovered);
    gl_FragColor = vec4(finalColor, 1.0);
}
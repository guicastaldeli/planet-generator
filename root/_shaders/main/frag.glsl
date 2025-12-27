precision mediump float;

#inclue "color.glsl"
#include "texture.glsl"

varying vec2 vTexCoord;
uniform float isHovered;

void main() {
    vec3 base = getBaseColor();
    vec3 hoverColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(base, hoverColor, isHovered);
    gl_FragColor = vec4(finalColor, 1.0);
}
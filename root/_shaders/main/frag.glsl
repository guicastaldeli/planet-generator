precision mediump float;

varying vec2 vTexCoord;
uniform float isHovered;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox.glsl"

void main() {
    vec3 base = getBaseColor();
    vec3 hoverColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(base, hoverColor, isHovered);
    gl_FragColor = vec4(finalColor, 1.0);
}
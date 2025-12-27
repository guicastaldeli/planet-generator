precision mediump float;

varying vec2 vTexCoord;
varying vec3 vViewDir;
varying vec3 vPos;
uniform float isHovered;
uniform float uTime;
uniform float shaderType;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox_frag.glsl"

void main() {
    if(shaderType > 0.5) {
        vec4 skyboxColor = getSkyboxColor(vViewDir, uTime);
        gl_FragColor = skyboxColor;
    } else {
        vec3 base = getBaseColor();
        vec3 hoverColor = vec3(1.0, 1.0, 1.0);
        vec3 finalColor = mix(base, hoverColor, isHovered);

        gl_FragColor = vec4(finalColor, 1.0);
    }
}
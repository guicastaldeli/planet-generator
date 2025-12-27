precision mediump float;

varying vec2 vTexCoord;
varying vec3 vViewDir;
uniform float isHovered;
uniform float uTime;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox_frag.glsl"

void main() {
    vec3 base = getBaseColor();
    vec3 hoverColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(base, hoverColor, isHovered);

    vec4 skyboxColor = getSkyboxColor(vViewDir, uTime);
    finalColor = mix(finalColor, skyboxColor.rgb, 0.5);

    gl_FragColor = vec4(finalColor, 1.0);
}
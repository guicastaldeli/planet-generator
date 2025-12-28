precision mediump float;

varying vec2 vTexCoord;
varying vec3 vViewDir;
varying vec3 vPos;
uniform float isHovered;
uniform float uTime;
uniform float shaderType;
varying vec2 vStarAttr;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox_frag.glsl"
#include "../skybox/star_color.glsl"

void main() {
    //Stars
    if(shaderType > 1.9 && shaderType < 2.1) {
        gl_FragColor = getStarColor(vColor, vStarAttr.y, uTime);
    }
    //Skybox
    else if(shaderType > 0.5) {
        vec4 skyboxColor = getSkyboxColor(vViewDir);
        gl_FragColor = skyboxColor;
    } 
    else {
        vec3 base = getBaseColor();
        vec3 hoverColor = vec3(1.0, 1.0, 1.0);
        vec3 finalColor = mix(base, hoverColor, isHovered);

        gl_FragColor = vec4(finalColor, 1.0);
    }
}
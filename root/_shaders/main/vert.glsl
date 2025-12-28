precision mediump float;

attribute vec3 aPos;
attribute vec2 aTexCoord;
attribute vec3 aColor;
attribute float aPhase;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float shaderType;

#include "color.glsl"
#include "../skybox/skybox_vert.glsl"
#include "../skybox/star_attr.glsl"

varying vec2 vTexCoord;
varying vec3 vViewDir;
varying vec3 vPos;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vTexCoord = aTexCoord;

    if(shaderType > 1.9 && shaderType < 2.1) {
        vColor = aColor;
        gl_PointSize = starAttr.x * 100.0;
        vStarAttr = starAttr;
        vPhase = aPhase;
    } else {
        vColor = pColor;
        vPhase = 0.0;
    }

    vec4 worldPos = model * vec4(aPos, 1.0);
    vViewDir = getSkyboxDir(worldPos.xyz);

}
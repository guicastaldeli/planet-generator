precision mediump float;

attribute vec3 aPos;
attribute vec2 aTexCoord;
attribute vec3 aColor;
attribute float aPhase;
attribute vec3 aNormal;

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
varying vec3 vNormal;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vTexCoord = aTexCoord;
    vPos = vec3(model * vec4(aPos, 1.0));
    vNormal = normalize(vec3(model * vec4(aNormal, 0.0)));

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
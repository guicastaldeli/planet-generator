precision mediump float;

varying vec2 vTexCoord;
varying vec3 vViewDir;
varying vec3 vPos;
varying vec3 vNormal;
varying vec2 vStarAttr;
varying float vPhase;

uniform float isHovered;
uniform float uTime;
uniform float shaderType;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox_frag.glsl"
#include "../skybox/star_color.glsl"
#include "../lightning/ambient_light.glsl"
#include "../lightning/point_light.glsl"

uniform int uNumPointLights;
uniform PointLight uPointLights[15];

void main() {
    //Stars
    if(shaderType > 1.9 && shaderType < 2.1) {
        gl_FragColor = getStarColor(
            vColor, 
            vStarAttr.y, 
            uTime,
            vPhase
        );
    }
    //Skybox
    else if(shaderType > 0.5) {
        vec4 skyboxColor = getSkyboxColor(vViewDir);
        gl_FragColor = skyboxColor;
    } 
    else {
        vec3 base = getBaseColor();
        
        Material material;
        material.ambient = base * 0.2;
        material.diffuse = base;
        material.specular = vec3(0.0, 0.0, 0.0);
        material.shininess = 0.0;

        Light light;
        light.position = vec3(0.0, 5.0, 5.0);
        light.color = vec3(0.1333, 0.1333, 0.1333);
        light.intensity = 1.0;

        vec3 normal = normalize(vNormal);
        vec3 viewDir = normalize(-vPos);
        vec3 lightning = calcPhongLightning(
            vPos,
            normal,
            viewDir,
            light,
            material
        );

        for(int i = 0; i < 15; i++) {
            if(i >= uNumPointLights) break;
            lightning += calcPointLight(
                uPointLights[i],
                material,
                vPos,
                normal,
                viewDir
            );
        }

        vec3 hoverColor = vec3(1.0, 1.0, 1.0);
        vec3 finalColor = mix(lightning, hoverColor, isHovered * 0.3);
        finalColor = min(finalColor, vec3(1.0));

        gl_FragColor = vec4(finalColor, 1.0);
    }
}
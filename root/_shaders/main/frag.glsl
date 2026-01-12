precision mediump float;

varying vec2 vTexCoord;
varying vec3 vViewDir;
varying vec3 vPos;
varying vec3 vNormal;
varying vec2 vStarAttr;

uniform float isHovered;
uniform float uTime;
uniform float shaderType;
uniform float uEmissiveStrength;
uniform float uEffectType;
uniform float uPlanetSize;
uniform float uIsAtmosphere;
varying float vPhase;

#include "color.glsl"
#include "texture.glsl"
#include "../skybox/skybox_frag.glsl"
#include "../skybox/star_color.glsl"
#include "../lightning/ambient_light.glsl"
#include "../lightning/point_light.glsl"
#include "../effect/fresnel.glsl"
#include "../effect/noise.glsl"
#include "../effect/glow.glsl"

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
        //Atmosphere
        if(uIsAtmosphere > 0.5) {
            vec3 normal = normalize(vNormal);
            vec3 viewDir = normalize(-vPos);

            float fresnel = 1.0 - max(dot(normal, viewDir), 0.0);
            fresnel = pow(fresnel, 2.0);

            vec3 base = getBaseColor();
            float alpha = fresnel * 0.1;
            vec3 glow = base * fresnel * 0.3;

            gl_FragColor = vec4(base + glow, alpha);
        }
        else {
            vec3 base = getBaseColor();
            vec3 normal = normalize(vNormal);
            vec3 viewDir = normalize(-vPos);

            //Glow
            vec3 glowResult = applyGlow(
                base,
                normal,
                viewDir,
                uEmissiveStrength,
                uTime,
                uPlanetSize
            );

            //Noise
            if(uEffectType > 1.5 && uEffectType < 2.5) {
                base = applyTurbulence(
                    base,
                    normal,
                    vPos,
                    uPlanetSize,
                    uTime
                );
            }
            
            //Default
            Material material;
            material.ambient = base * 0.2;
            material.diffuse = base;
            material.specular = vec3(0.0, 0.0, 0.0);
            material.emissive = uEmissiveStrength > 0.0 ? glowResult : vec3(0.0);
            material.shininess = 0.0;

            Light light;
            light.position = vec3(0.0, 5.0, 5.0);
            light.color = vec3(0.0431, 0.0431, 0.0431);
            light.intensity = 1.0;

            AmbientLight ambientLightUniform;
            ambientLightUniform.color = uAmbientLight.color;
            ambientLightUniform.intensity = uAmbientLight.intensity;
            ambientLightUniform.enabled = bool(uAmbientLight.enabled);

            vec3 lightning;
            if(ambientLightUniform.enabled == true) {
                lightning = calcPhongLightning(
                    vPos,
                    normal,
                    viewDir,
                    light,
                    ambientLightUniform,
                    material
                );
            }

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

            vec3 litColor = lightning + material.emissive;
            vec3 finalColor = mix(litColor, hoverColor, isHovered * 0.3);
            finalColor = min(finalColor, vec3(1.0));

            gl_FragColor = vec4(finalColor, 1.0);
        }
    }
}
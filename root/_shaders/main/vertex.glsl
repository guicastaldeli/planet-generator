attribute vec3 aPos;
attribute vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#include "color.glsl"

varying vec2 vTexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
    vColor = pColor;
}
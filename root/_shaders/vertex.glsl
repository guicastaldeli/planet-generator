attribute vec3 aPos;
attribute vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 pColor;
varying vec3 vColor;
varying vec2 vTexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vColor = pColor;
    vTexCoord = aTexCoord;
}
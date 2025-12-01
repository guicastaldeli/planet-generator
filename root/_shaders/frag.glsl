precision mediump float;
uniform float isHovered;

void main() {
    vec3 original = vec3(1.0, 0.5, 0.2);
    vec3 hover = vec3(1.0, 1.0, 1.0);
    vec3 color = mix(original, hover, isHovered);
    gl_FragColor = vec4(color, 1.0);
}
precision mediump float;

varying vec3 vColor;
uniform float isHovered;

void main() {
    vec3 baseColor = vColor;
    vec3 hoverColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(baseColor, hoverColor, isHovered);
    gl_FragColor = vec4(finalColor, 1.0);
}
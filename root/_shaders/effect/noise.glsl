float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
        (c - a) * u.y * (1.0 - u.x) +
        (d - b) * u.x * u.y;
}

float fbm(vec2 st) {
    float val = 0.0;
    float ampl = 0.5;
    float freq = 1.0;

    val += ampl * noise(st * freq);
    freq *= 2.0;
    ampl *= 0.5;
    
    val += ampl * noise(st * freq);
    freq *= 2.0;
    ampl *= 0.5;
    
    val += ampl * noise(st * freq);
    freq *= 2.0;
    ampl *= 0.5;
    
    val += ampl * noise(st * freq);
    freq *= 2.0;
    ampl *= 0.5;
    
    val += ampl * noise(st * freq);
    freq *= 2.0;
    ampl *= 0.5;
    
    val += ampl * noise(st * freq);
    
    return val;
}

vec3 applyTurbulence(
    vec3 baseColor,
    vec3 normal,
    vec3 worldPos,
    float planetSize,
    float uTime
) {
    vec3 norm = normalize(normal);
    
    float u = atan(norm.x, norm.z) / (2.0 * 3.14159265359) + 0.5;
    float v = norm.y * 0.5 + 0.5;
    vec2 uv = vec2(u, v);
    
    vec2 animatedUV = uv;
    animatedUV.x += uTime * 20.0;
    
    float distort1 = fbm(animatedUV * 3.0 + vec2(0.0, uTime * 0.01));
    float distort2 = fbm(animatedUV * 3.0 + vec2(100.0, -uTime * 0.015));
    
    vec2 distortedUV = animatedUV;
    distortedUV.x += sin(animatedUV.y * 10.0 + distort1 * 6.28) * 0.1;
    distortedUV.y += sin(animatedUV.x * 8.0 + distort2 * 6.28) * 0.05;
    
    float vortexNoise = fbm(distortedUV * 8.0);
    distortedUV.x += cos(vortexNoise * 6.28 + uTime * 0.1) * 0.08;
    distortedUV.y += sin(vortexNoise * 6.28 + uTime * 0.08) * 0.08;
    
    float turbulence = fbm(distortedUV * 15.0);
    float bands = sin(v * 18.0 + fbm(vec2(u * 5.0, v * 3.0)) * 3.0) * 0.5 + 0.5;
    turbulence += fbm(distortedUV * 25.0 + vec2(uTime * 0.03, -uTime * 0.02)) * 0.5;
    
    vec2 detailUV = distortedUV * 40.0;
    detailUV.x += fbm(detailUV * 0.5) * 0.5;
    detailUV.y += fbm(detailUV * 0.5 + vec2(50.0, 50.0)) * 0.5;

    turbulence += fbm(detailUV) * 0.3;
    turbulence = turbulence * 0.7 + bands * 0.3;
    turbulence = clamp(turbulence, 0.0, 1.0);
    turbulence = pow(turbulence, 0.75);

    vec3 result = baseColor * (0.35 + turbulence * 1.3);
    return result;
}
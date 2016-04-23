// from https://www.shadertoy.com/view/4stXWB

float hash(float n) {
    return fract(sin(n)*4537.3432);
}

float noise(vec2 x) {
    vec2 p = floor(x);
    vec2 f = fract(x);

    f = f*f*(3.0*f - 2.0);

    float n = p.x + p.y*57.0;

    return mix(
          mix(hash(n), hash(n + 1.0), f.x),
          mix(hash(n + 57.0), hash(n + 58.0), f.x),
          f.y);
}

float fbm(vec2 p) {
    float f = 0.0;

    f += 0.5000*noise(p); p *= 2.01;
    f += 0.2500*noise(p); p *= 2.03;
    f += 0.1250*noise(p); p *= 2.02;
    f += 0.0625*noise(p);

    return f/0.9375;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 p = -1.5 + 3.0*(fragCoord.xy / iResolution.xy);
    p.x *= iResolution.x/iResolution.y;

    vec2 op = p;
    float or = length(p);

    p.x -= smoothstep(-.5, 5.0, cos(20.0*iGlobalTime) - sin(5.*iGlobalTime));
    p.y += smoothstep(-0.0, 5.0, cos(4.*iGlobalTime));
    //p.y += fract(iGlobalTime);
    float r = length(p);
    float a = atan(p.y, p.x);

    float f = smoothstep(0.7, 1., fbm(vec2(r, 12.*a)));
    vec3 bg = mix(vec3(1), vec3(1, 0, 0), f);

    f = smoothstep(0.9, 2.1, or);
    bg *= 1.0 - f;

    vec3 col = bg;

    if(r < 0.8) {
        col = vec3(.1, .3, .4);

        f = smoothstep(0.0, 1.0, fbm(2.*p));
        col = mix(col, vec3(.1, .5, .3), 0.6*f);

        f = smoothstep(0.0, .2, mod(a, .08)*r);
        col *= 1.0 - f;

        f = smoothstep(0.2, .2 + abs(cos(iGlobalTime))*.5, r);
        col *= f;

        f = smoothstep(0.6, .8, r);
        col *= mix(col, vec3(1), 1.0 - 0.9*f);

        f = smoothstep(.75, .8, r);
        col = mix(col, bg, f);

        f = smoothstep(.0, .4, length((0.4*op - p) + vec2(.25)));
        col = mix(col, vec3(1), 1.0 - f);
    }
  fragColor = vec4(col, 1.0);
}

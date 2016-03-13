#version 400 core

//define fragCoord globally, simple solution to reatin compatibility with shadertoy
vec2 fragCoord = gl_FragCoord.xy;

// ouput
out vec4 fragColor;

//----------------------------------------------------------------------------------------------------------------//

//inputs are uniforms set in correct format for shadertoy to interface with
uniform vec3 iResolution;
uniform float iGlobalTime;
uniform float iTimeDelta;

//currently frame rate is locked to 60fps, probably should be variable or something
//incremented every time the frame renders, in paintGL
uniform int iFrame;

//uniform float iChannelTime[4];
//uniform float iChannelResolution[4];
uniform vec4 iMouse;

//need to set up multiple channels, maybe they ought to be flexible, although this could be done outside of shader
uniform sampler2D iChannel0;

//date works correctly, using QDateTime variable
uniform vec4 iDate;

//to do with audio?
//uniform float iSampleRate;

//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
/*
vec4 circle(vec2 uv, vec2 center, float rad, vec3 color)
{
  if(length(center-uv) < rad) return vec4(color, 1.0);
  else return vec4(color, 0.0);
}

void main()
{
  vec2 uv = fragCoord.xy;
  uv.x /= 1024.0;
  uv.x *= 1024.0/720.0;
  uv.y /= 720.0;

  //fragColor = vec4(0.0);
  //fragColor.r = uv.x;
  //fragColor.g = uv.y;

  vec2 center = vec2(0.5, 0.5);

  float radius = 0.25;

  vec4 layer1 = vec4(0.7, 0.8, 0.8, 1.0);

  vec3 red = vec3(1.0, 0.4, 0.3);

  vec4 layer2 = circle(uv, center, radius, red);
  fragColor = mix(layer1, layer2, layer2.a);
}
*/
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
/*
const int max_iterations = 45;
const float stop_threshold = 0.001;
const float grad_step = 0.001;
const float clip_far = 4.0;
const float PI = 3.14159265359;

const float furDepth = 0.5;
const int furLayers = 50;
const float rayStep = furDepth * 2.0 / float(furLayers);
const float furThreshold = 0.4;
const float uvScale = 0.5;

float r = 1.;
float rotation = 60. * (iGlobalTime);
float elevation = 0.25 * cos(iGlobalTime * 2.5);


mat3 rotX(float g) {
  g = radians(g);
  vec2 a = vec2(cos(g), sin(g));
  return mat3(1.0, 0.0, 0.0,
        0.0, a.x, -a.y,
        0.0, a.y, a.x);
}

mat3 rotY(float g) {
  g = radians(g);
  vec2 a = vec2(cos(g), sin(g));
  return mat3(a.x, 0.0, a.y,
        0.0, 1.0, 0.0,
        -a.y, 0.0, a.x);
}

vec3 map(vec3 p) {
  p.y -= elevation;
  p *= rotY(rotation);
  float s = (length(p) - r) - 0.12 * sin(6. * p.x) * cos(6. * p.y)* sin(6. * p.z);
  return vec3(s * 0.5);
}


vec2 cartesianToSpherical(vec3 p) {
  float r = length(p);
  p = normalize(p);
  float t = (r - (1.0 - furDepth)) / furDepth;
  p.zyx *= rotX(rotation);
  vec2 uv = vec2(atan(p.y, p.x), acos(p.z));

  uv.y -= t * t * elevation;

  return uv;
}

float furDensity(vec3 pos) {
  vec2 uv = cartesianToSpherical(pos.xzy);
  //vec4 tex = texture2D(iChannel0, uv * uvScale);
  //float density = smoothstep(furThreshold, 1.0, tex.x);
  float density = 1.0;
  pos.y -= elevation;
  vec3 p = pos;
  p *= rotY(rotation);
  float r = length(pos) - 0.12 * sin(6. * p.x) * cos(6. * p.y)* sin(6. * p.z);;
  float t = (r - (1. - furDepth)) / furDepth;

  return density * (1. - t);

}

vec3 furNormal(vec3 pos, float density) {
    float eps = 0.0001;
    vec3 n;
    n.x = furDensity( vec3(pos.x + eps, pos.y, pos.z)) - density;
    n.y = furDensity( vec3(pos.x, pos.y + eps, pos.z)) - density;
    n.z = furDensity( vec3(pos.x, pos.y, pos.z + eps)) - density;
    return normalize(n);
}


vec3 furShade(vec3 pos, vec3 color, vec3 light, vec3 eye, float density) {

  vec3 v = normalize(light - pos);
  vec3 n = furNormal(pos, density);
  vec3 ev = normalize(pos - eye);
  vec3 h = reflect(ev, n);

  float diff = max(0.0, dot(v, n)) + 0.4;
  float spec = pow(max(0.0, dot(v, h)), 64.);

  float r = length(pos);
  float t = (r - (1.0 - furDepth)) / furDepth;
  t = clamp(t, 0.3, 1.);

  diff = mix(diff, 1., 0.5);

  return color * t * (diff + 1.9 * spec);
}

vec3 gradient( vec3 v ) {
  const vec3 delta = vec3( grad_step, 0.0, 0.0 );
  float va = map( v ).x;
  return normalize (
    vec3(
      map( v + delta.xyy).x - va,
      map( v + delta.yxy).x - va,
      map( v + delta.yyx).x - va
    )
  );
}

vec3 ray_marching( vec3 origin, vec3 dir, float start, float end ) {

  float depth = start;
  vec3 salida = vec3(end);
  vec3 dist = vec3(0.1);

  for ( int i = 0; i < max_iterations; i++ ) 		{
    if ( dist.x < stop_threshold || depth > end ) break;
              dist = map( origin + dir * depth );
              depth += dist.x;
  }

  salida = vec3(depth, dist.y, dist.z);
  return salida;
}

vec3 shading( vec3 v, vec3 n, vec3 eye, vec3 lightMix) {

  vec3 final = vec3( 0.0 );
  vec3 ev = normalize( v - eye );
  vec3 ref_ev = reflect( ev, n );
  vec3 light_pos   = vec3(0.0, 2.0, -2.0);
  vec3 vl = normalize( light_pos - v );
  float diffuse  = max( 0.0, dot( vl, n ) );
  float specular = max( 0.0, dot( vl, ref_ev ) );
  specular = pow( specular, lightMix.x );
  final += vec3( 0.9 ) * ( diffuse * lightMix.y + specular * lightMix.z);
  final += vec3(0.2);

  return final;
}

vec3 ray_dir( float fov, vec2 size, vec2 pos ) {
  vec2 xy = pos - size * 0.5;

  float cot_half_fov = tan(radians( 90.0 - fov * 0.5 ));
  float z = size.y * 0.5 * cot_half_fov;

  return normalize( vec3( xy, z ) );
}


void main()
{
  vec3 rd = ray_dir(60.0, iResolution.xy, fragCoord.xy );

  vec3 eye = vec3( .0, .0, -2.8 );

  vec3 lightMix = vec3(6., 0.45, 0.15);
  vec3 color = vec3(0.);

  vec3 data = ray_marching( eye, rd, 0.0, clip_far );
  if ( data.x < clip_far ) {

    vec3 pos = eye + rd * data.x;
    vec3 n = gradient( pos );

    vec3 lightColor =  shading( pos, n, eye, lightMix) * 2.0;
    color = vec3(1.) * lightColor;

    vec4 c = vec4(0.0);

    for(int i=0; i<furLayers; i++) {
      vec4 sampleCol;
      sampleCol.a = furDensity(pos);
      if( c.a > 0.99 ) break;
      if (sampleCol.a > 0.) {
        sampleCol.rgb = sampleCol.a * furShade(pos, vec3(85., 72., 56) * lightColor / 255., vec3(0.0, 2.0, -2.0), eye, sampleCol.a);
        c = c + sampleCol * (1.0 - c.a);
      }
      pos += rd * rayStep;
    }

    color = c.xyz;
  }

  fragColor = vec4(2. * color, 1.0 );
}
*/
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
/*
// Bump up the iterations!
// More iterations means more detail + aliasing
#define ITERATIONS 20

void main() {
    float mouseDown = step(.05, iMouse.z);
    float scale = mix(1., .4, mouseDown);
    vec2 look = (iMouse.xy / iResolution.xy - .5) * 3. * mouseDown;

    float time = iGlobalTime * 2. + 15.;
    vec2 res = iResolution.xy;
    vec2 uv = fragCoord.xy / res - vec2(.5) + look;
    uv *= vec2(res.x / res.y, 1.) * 4. * scale;

    float len = dot(uv, uv) * .3 - .4;

    vec3 z = sin(time * vec3(.23, .19, .17));
    for (int i = 0; i < ITERATIONS; i++) {
        z += cos(z.zxy + uv.yxy * float(i) * len);
    }

    float val = z.r * .06 + .3;
    val -= smoothstep(.1, -.3, len) * 1.5 + len * .3 - .4;
    fragColor = vec4(vec3(max(val, .1)), 1.);
}
*/
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
  vec2 pa = p-a, ba = b-a;
  float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
  return length( pa - ba*h );
}

vec3 line( in vec3 buf, in vec2 a, in vec2 b, in vec2 p, in vec2 w, in vec4 col )
{
   float f = sdSegment( p, a, b );
   float g = fwidth(f)*w.y;
   return mix( buf, col.xyz, col.w*(1.0-smoothstep(w.x-g, w.x+g, f)) );
}

vec3 hash3( float n ) { return fract(sin(vec3(n,n+1.0,n+2.0))*43758.5453123); }

void main()
{
    // get time
    float mils = fract(iDate.w);
  float secs = mod( floor(iDate.w),        60.0 );
  float mins = mod( floor(iDate.w/60.0),   60.0 );
  float hors = mod( floor(iDate.w/3600.0), 24.0 );

    // enable this for subsecond resolution
    //secs += mils;

  vec2 uv = (2.0*fragCoord.xy-iResolution.xy)/min(iResolution.y,iResolution.x);

  float r = length( uv );
  float a = atan( uv.y, uv.x )+3.1415926;

  // background color
  vec3 nightColor = vec3( 0.2, 0.2, 0.2 ) + 0.1*uv.y;
  vec3 dayColor   = vec3( 0.5, 0.6, 0.7 ) + 0.2*uv.y;
  vec3 col = mix( nightColor, dayColor, smoothstep( 5.0, 7.0, hors ) -
                                  smoothstep(19.0,21.0, hors ) );

    // inner watch body
  col = mix( col, vec3(0.9-0.4*pow(r,4.0)), 1.0-smoothstep(0.94,0.95,r) );

    // 5 minute marks
  float f = abs(2.0*fract(0.5+a*60.0/6.2831)-1.0);
  float g = 1.0-smoothstep( 0.0, 0.1, abs(2.0*fract(0.5+a*12.0/6.2831)-1.0) );
  float w = fwidth(f);
  f = 1.0 - smoothstep( 0.1*g+0.05-w, 0.1*g+0.05+w, f );
  f *= smoothstep( 0.85, 0.86, r+0.05*g ) - smoothstep( 0.94, 0.95, r );
  col = mix( col, vec3(0.0), f );

  // seconds hand
  vec2 dir;
  dir = vec2( sin(6.2831*secs/60.0), cos(6.2831*secs/60.0) );
    col = line( col, vec2(0.0), dir*0.9, uv+0.05, vec2(0.005,4.0), vec4(0.0,0.0,0.0,0.2) );
    col = line( col, vec2(0.0), dir*0.9, uv,      vec2(0.005,1.0), vec4(0.5,0.0,0.0,1.0) );

  // minutes hand
  dir = vec2( sin(6.2831*mins/60.0), cos(6.2831*mins/60.0) );
    col = line( col, vec2(0.0), dir*0.7, uv+0.05, vec2(0.015,4.0), vec4(0.0,0.0,0.0,0.2) );
    col = line( col, vec2(0.0), dir*0.7, uv,      vec2(0.015,1.0), vec4(0.0,0.0,0.0,1.0) );

    // hours hand
  dir = vec2( sin(6.2831*hors/12.0), cos(6.2831*hors/12.0) );
    col = line( col, vec2(0.0), dir*0.4, uv+0.05, vec2(0.015,4.0), vec4(0.0,0.0,0.0,0.2) );
    col = line( col, vec2(0.0), dir*0.4, uv,      vec2(0.015,1.0), vec4(0.0,0.0,0.0,1.0) );

    // center mini circle
  col = mix( col, vec3(0.5), 1.0-smoothstep(0.050,0.055,r) );
  col = mix( col, vec3(0.0), 1.0-smoothstep(0.005,0.01,abs(r-0.055)) );

    // border of watch
  col = mix( col, vec3(0.0), 1.0-smoothstep(0.01,0.02,abs(r-0.95)) );

    // dithering
    col += (1.0/255.0)*hash3(uv.x+13.0*uv.y);

  fragColor = vec4( col,1.0 );
}

//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
/*
void main()
{
  vec2 uv = fragCoord.xy / iResolution.xy;

  fragColor =  texture2D(iChannel0, uv);
}
*/

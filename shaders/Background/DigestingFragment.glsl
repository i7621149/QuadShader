// from https://www.shadertoy.com/view/lddXWB
// uses tex16.png

// Uncomment ENABLE_AA to make things even slower.
// Lots of utility code in here by IQ.

#define ULTRA_DETAIL
//#define ENABLE_AA

float amount_balls = 1.0000;
float amount_ribs = 0.0000;
float amount_smooth = 0.0000;
float amount_fresnel = 0.0696;
float amount_remove = 0.0000;
float amount_bottomlight = 0.1000;
float specular_exp = 4.0000;

vec3 clr_bg = vec3( 0.0 );
vec3 clr_base = vec3( 0.0 );
vec3 clr_ambient = vec3( 0.0100, 0.0100, 0.0100 );
vec3 clr_remove = vec3( 0.3000, 0.3000, 0.3000 );
vec3 clr_spec = vec3( 1.0000, 1.0000, 1.0000 );
vec3 clr_skylight = vec3( 1.0000, 0.9608, 0.8431 );
vec3 clr_diffuse = vec3( 1.0000, 0.0000, 0.0000 );
vec3 clr_subsurface = vec3( 0.3176, 0.1059, 0.5608 );
vec3 clr_lights_ambient = vec3( 0.4000, 0.4000, 0.4000 );
vec3 clr_lights_diffuse = vec3( 1.0000, 1.0000, 1.0000 );
vec3 clr_glow = vec3( 1.0000, 1.0000, 1.0000 );

float glow_dist = 0.1000;
float glow_amount = 0.0000;
float show_lights = 1.0000;

vec3 cam_mindist = vec3( -7.3140, -7.7585, -5.9952 );
vec3 cam_floatdist = vec3( 1.0000, 1.0000, 0.5000 );

#ifdef ENABLE_AA
#define AA_ITER_COUNT 9
#define AA_ITER_COUNT_SQRT 3.0
#else
#define AA_ITER_COUNT 1
#define AA_ITER_COUNT_SQRT 1.0
#endif

#define AO_ITERATIONS	32

#define MAX_DIST	1000.0
#define MATERIAL_NONE	0.0
#define MATERIAL_MAIN	1.0
#define MATERIAL_LIGHTS	100.0

// Globals
float time0;

vec3 hash3( float n )
{
  return fract(sin(vec3(n,n+1.0,n+2.0))*23421.7578287);
}

float hash1f( float f )
{
  return fract(.5 + .5 * sin(f)*12859.2342608);
}

float smin( float a, float b, float k )
{
  float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
  return mix( b, a, h ) - k*h*(1.0-h);
}

float sdSphere( vec3 p, float r )
{
  return length(p) - r;
}

float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}

// iq fast noise
float noisefast( vec3 p )
{
  vec3 ip = floor( p ), f = fract( p );
    f = f * f * f * ( f * ( f * 6.0 - 15.) + 10.0 );

  vec2 uv = ( ip.xy + vec2(37.0,17.0)*ip.z) + f.xy;
  vec2 rg = texture2D( iChannel0, (uv + 0.5)/256.0, -100.0 ).yx;

  return mix(rg.x, rg.y, f.z);
}

float noise11( in float x )
{
  float p = floor(x);
  float f = fract(x);
  return mix( hash1f(p), hash1f(p+1.0), f*f*(3.0-2.0*f));
}

float n( vec3 p )
{
  return noisefast( p );
}

mat3 GetCameraRotationMatrix( in vec3 eye, in vec3 lookat, float roll )
{
  vec3 cz = normalize( lookat - eye );
  vec3 cp = vec3(sin( roll ), cos( roll ), 0.0);
  vec3 cx = normalize( cross(cz,cp) );
  vec3 cy = normalize( cross(cx,cz) );
  return mat3( cx, cy, cz );
}

float FBM( in vec3 p, in vec3 offset, float a, float f )
{
  float x = 0.0;
  x += a * n( p ); a *= 0.5; p = 2.01 * p - offset;
  x += a * n( p ); a *= 0.5; p = 2.03 * p - offset;
  x += a * n( p ); a *= 0.5; p = 2.02 * p - offset;
  x += a * n( p ); a *= 0.5; p = 2.01 * p - offset;
#ifdef ULTRA_DETAIL
  x += a * n( p ); a *= 0.5; p = 2.02 * p - offset;
  x += a * n( p ); a *= 0.5; p = 2.01 * p - offset;
  x += a * n( p ); a *= 0.5; p = 2.01 * p - offset;
#endif
  x += a * n( p );
  return x;
}

float Thingy( in vec3 p, float phase )
{
  float t = time0 + phase;
  float st = sin( t );
  float nst = 0.5 + 0.5 * sin( t );//sin( noise11( t ) );
  float v = sin( st + sin( 0.1 * p.x ) );
  float offset_r = mix( 0.1, 2.0, 0.75 + 0.25 * sin( time0 * 0.1 ) );
  vec3 q = p;
  float n11 = noise11( 2.0 * t );
  float yf = t + .1 * p.y + n11;
  float yf2 = t + .12 * p.y + noise11( 1.58 * t + 234234.3234 );
  q.xz += vec2( v * offset_r * cos( yf ), v * offset_r * sin( yf2 ) );

  float r = 0.6 + 0.3 * sin( q.y + t );
  float d = sdCylinder( q, vec3( r, r * 2.0, r ) );

  // Ribs
  float f = 3.0;
  d += amount_balls * 0.23 * nst * sin( .3 * t + f * q.x ) * sin( f * q.y ) * sin( f * q.z );
  d += amount_ribs * .004 * sin( .9 * t + 10. * f * q.y ) * sin( f * q.z );

  float sphere_y = mix( -13.0, 13.0, mod( 0.1 * t + sin( .05 * ( mix( -1.0, 1.0, noise11( t ) ) + t ) ), 5.0 ) );
  float sp = sdSphere( q + vec3( 0.0, sphere_y, 0.0 ), mix( 2.0, 2.35, nst ) - mod( phase, 0.1 ) );

  float k = mix( 1.0, 1.6, .5 + .5 * sin( .3 * t ) );
  float n = ( 1.0 - amount_smooth ) * FBM( q - vec3( 0.0, -time0, 0.0 ), mix( vec3( k ), vec3( cos( nst ) ), nst ), 0.5, 1.0 );
  n = pow( 0.5 + 0.5 * n, 2.0 );
  return smin( d, sp, k ) - n;
}

vec2 Map( in vec3 p )
{
  vec3 q = p;

  return vec2( Thingy( q, 0.0 ), MATERIAL_MAIN );
}

vec3 Normal( in vec3 p, float e )
{
  vec3 eps = vec3( e, 0.0, 0.0 );

  return normalize( vec3( Map(p+eps.xyy).x - Map(p-eps.xyy).x,
        Map(p+eps.yxy).x - Map(p-eps.yxy).x,
        Map(p+eps.yyx).x - Map(p-eps.yyx).x ) );
}

vec3 Render( vec2 uv, vec3 eye, vec3 dir )
{
  vec3 col = clr_base;

  float tmax = MAX_DIST;
  float h = 1.0;
  float t = 0.0;
  float m = 1e10;
  float mat = MATERIAL_NONE;
  vec2 min_result = vec2( 1e10, MATERIAL_NONE );

  for( int i=0; i<100; i++ )
  {
    vec2 result = Map( eye + t*dir );

    h = result.x;
    mat = result.y;

    if ( result.x < min_result.x )
    {
      min_result = result;
    }

    if( h < 0.0001 || t > tmax )
      break;

    t += h;
  }

  vec3 sunpos = vec3( 0.0, 5.0, 0.0 );
  vec3 lightdir = vec3( cos( time0 ), 1.0, sin( time0 ) );

  if( t<tmax )
  {
    vec3 pos = eye + t*dir;
    vec3 norm = Normal( pos, 0.005 );

    col += clr_ambient;

    float f = mix( 1.0, 10.0, noise11( 28342.223 + iGlobalTime ) );
    float occ = mix( 0.0, 1.1, noise11( f * iGlobalTime ) );

    // diffuse
    float diff = 0.0;
    for( int i=0; i<AO_ITERATIONS; i++ )
    {
      vec3 rr = normalize(-1.0 + 2.0*hash3(float(i)*123.5463));
      rr = normalize( norm + 7.0*rr );
      rr = rr * sign(dot(norm,rr));
      diff += 4.0 * 0.1 * dot(rr,norm);

    }
    diff /= float(AO_ITERATIONS);

    diff *= 4.0;

    // material
    if ( mat == MATERIAL_MAIN )
    {
      vec3 ref = reflect( lightdir, norm );
      float spec = clamp( dot( normalize( eye - pos ), ref ), 0.0, 1.0 );
      float fres = clamp( 1.0 + dot( norm, dir ), 0.0, 1.0 );
      float diff_dot = clamp( dot(norm,lightdir), 0.0, 1.0 );
      col += occ * diff * clr_diffuse * diff_dot;

      col += clr_skylight * clamp( norm.y, 0.0, 1.0 );	// sky light

      float s = .5 + .5 * sin( .5 * iGlobalTime );
      col.r -= amount_remove * smoothstep( -3.0, 3.0, pos.x ) * clr_remove.r * s;
      col.g -= amount_remove * smoothstep( -3.0, 3.0, pos.x ) * clr_remove.g * s;
      col.b -= amount_remove * smoothstep( -3.0, 3.0, pos.z ) * clr_remove.b * s;

      // glow
      col += glow_amount * clr_glow;
    }
    else if ( mat >= MATERIAL_LIGHTS && show_lights > 0.0 )
    {
      col += clr_lights_ambient;
      col += diff * clr_lights_diffuse * clamp( dot( norm, lightdir ), 0.0, 1.0 );

      col *= show_lights;
    }

    col *= exp( -0.050*t );	// fog
  }
  else
  {
    col = clr_bg;
  }

  return col;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  float aspect = iResolution.x / iResolution.y;

  time0 = 25. + iGlobalTime;
  time0 += 2.0 * smoothstep( 0.00,  1.5, iGlobalTime );
  time0 += 2.0 * smoothstep( 10.0, 15.0, iGlobalTime );

  vec3 col = vec3( 0.0 );

#ifdef ENABLE_AA
  for ( int i = 0; i < AA_ITER_COUNT; ++i )
#else
  int i = 0;
#endif
  {
    vec2 offset = vec2( float(i) / AA_ITER_COUNT_SQRT, mod( float(i), AA_ITER_COUNT_SQRT ) ) / AA_ITER_COUNT_SQRT;
    vec2 uvs = -1.0 + 2.0 * ( fragCoord.xy + offset ) / iResolution.xy;
        uvs.x *= aspect;

    // CAMERA

    float camspeed = 0.1;
        float look_phase = 0.03 * iGlobalTime + 3.14159 * 2.0 * iMouse.x/iResolution.x;
        float df = mix( 1.0, 1.7, iMouse.y/iResolution.y );
    vec3 eye = vec3(
      df * cam_mindist.x * cos( look_phase ) + cam_floatdist.x * cos( 1.5708 + camspeed * 0.10 * time0 ),
      df * cam_mindist.y + cam_floatdist.y * cos( camspeed * 0.011 * time0 ),
      df * cam_mindist.z * sin( look_phase ) + cam_floatdist.z * sin( camspeed * 0.10 * time0 )
    );

    mat3 ca = GetCameraRotationMatrix( eye, vec3( 0.0 ), 0.0 );

    // ray direction
    vec3 dir = ca * normalize( vec3(uvs.xy,3.0) );

    col += Render( uvs, eye, dir );
  }

  col /= float( AA_ITER_COUNT );

  // POSTPROCESSING

  // contrast
  col = mix( col, smoothstep( 0.0, 1.0, col ), 0.1 );

  // dithering
  col += ( 25.0 / 255.0 )*hash3( iGlobalTime + fragCoord.x + 300.0 * fragCoord.y );

    // scanlines
    float f = noise11( ( 300.0 + iGlobalTime ) * ( 0.2 + fragCoord.y * 0.1 ) );
    vec2 uv = fragCoord.xy / iResolution.xy;
    f *= smoothstep( 0.0, 0.5, uv.y );
  col.r -= 0.20 * f;
    col.g *= 1.0 + 0.1 * f * f + 0.05 * uv.y;
    col.b -= 0.1 * ( 1.0 - uv.y );
    col.r += 0.03 * uv.x;
    col = pow( col, vec3( mix( 0.9, 1.0, f ) ) );

  fragColor = vec4( mix( col, 1.0 - col, 0.1 * 0.7 * noise11( 30.0 * iGlobalTime ) ), 1.0 );
}

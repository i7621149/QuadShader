// from https://www.shadertoy.com/view/MstSDB

/*** Sceneradio@Wappuradio live coding ***

If any of the viewers wish to know more about coding raymarchers there's great tutorial at
https://www.shadertoy.com/view/XllGW4 Basic functions are copied from that tutorial.

For more additional info visit http://iquilezles.org/www/index.htm

This is 2 hour live shader coding during radio show.
For the template used visit: https://www.shadertoy.com/view/MdtSDB

Actual code starts from line 385
*/
// __ Matrix functions __ _____________________________________

    // Return 2x2 rotation matrix
    // With vector swizzle/mask can use as a 3x3 xform
    // For y, you need to invert 
    // angle in radians
    // ========================================
    mat2 Rot2(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat2( c, -s, s, c );
    }

    // http://www.songho.ca/opengl/gl_anglestoaxes.html

    // Return 4x4 rotation X matrix
    // angle in radians
    // ========================================
    mat4 Rot4X(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4( 1, 0, 0, 0,
                     0, c,-s, 0,
                     0, s, c, 0,
                     0, 0, 0, 1 );
    }

    // Return 4x4 rotation Y matrix
    // angle in radians
    // ========================================
    mat4 Rot4Y(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4( c, 0, s, 0,
                     0, 1, 0, 0,
                    -s, 0, c, 0,
                     0, 0, 0, 1 );
    }

    // Return 4x4 rotation Z matrix
    // angle in radians
    // ========================================
    mat4 Rot4Z(float a ) {
        float c = cos( a );
        float s = sin( a );
        return mat4(
            c,-s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
         );
    }

    // Translate is simply: p - d
    // opTx will do transpose(m)
    // p' = m*p
    //    = [m0 m1 m2 m3 ][ p.x ]
    //      [m4 m5 m6 m7 ][ p.y ]
    //      [m8 m9 mA mB ][ p.z ]
    //      [mC mD mE mF ][ 1.0 ]
    // ========================================
    mat4 Loc4( vec3 p ) {
        p *= -1.;
        return mat4(
            1,  0,  0,  p.x,
            0,  1,  0,  p.y,
            0,  0,  1,  p.z,
            0,  0,  0,  1
        );
    }

    // if no support for GLSL 1.2+
    //     #version 120
    // ========================================
    mat4 transpose(in mat4 m ) {
        vec4 r0 = m[0];
        vec4 r1 = m[1];
        vec4 r2 = m[2];
        vec4 r3 = m[3];

        mat4 t = mat4(
             vec4( r0.x, r1.x, r2.x, r3.x ),
             vec4( r0.y, r1.y, r2.y, r3.y ),
             vec4( r0.z, r1.z, r2.z, r3.z ),
             vec4( r0.w, r1.w, r2.w, r3.w )
        );
        return t;
    }
// __ Smoothing functions _____________________________________

    // Smooth Min
    // http://www.iquilezles.org/www/articles/smin/smin.htm

    // Min Polynomial
    // ========================================
    float sMinP( float a, float b, float k ) {
        float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
        return mix( b, a, h ) - k*h*(1.0-h);
    }

    // Min Exponential
    // ========================================
    float sMinE( float a, float b, float k) {
        float res = exp( -k*a ) + exp( -k*b );
        return -log( res )/k;
    }

    // Min Power
    // ========================================
    float sMin( float a, float b, float k ) {
        a = pow( a, k );
        b = pow( b, k );
        return pow( (a*b) / (a+b), 1.0/k );
    }

// __ Surface Primitives ____________________________

    // Return max component x, y, or z
    // ========================================
    float maxcomp(in vec3 p ) {
        return max(p.x,max(p.y,p.z));
    }

// Signed

    // b.x = Width
    // b.y = Height
    // b.z = Depth
    // Leave r=0 if radius not needed
    // ========================================
    float sdBox(vec3 p, vec3 b, float r) {
        vec3 d = abs(p) - b;
        return min(maxcomp(d),0.0) - r + length(max(d,0.0));
        // Inlined maxcomp
        //return min(max(d.x,max(d.y,d.z)),0.0) - r + length(max(d,0.0));
    }

    // ========================================
    float sdCappedCylinder( vec3 p, vec2 h ) {
        vec2 d = abs(vec2(length(p.xz),p.y)) - h;
        return min(max(d.x,d.y),0.0) + length(max(d,0.0));
    }

    // ========================================
    float sdCapsule( vec3 p, vec3 a, vec3 b, float r ) {
        vec3 pa = p - a, ba = b - a;
        float h = clamp( dot(pa,ba) / dot(ba,ba), 0.0, 1.0 );
        return length( pa - ba*h ) - r;
    }

    // c.x Width
    // c.y Base Radius
    // c.z Depth
    // Note: c must be normalized
    // ========================================
    float sdCone( vec3 p, vec3 c) // TODO: do we need to use 'in' for all primitives?
    {
        // c.x = length
        // c.y = base radius
        //float q = length( p.xy );
        //return dot( c, vec2( q, p.z ) ); // BUG in iq's docs -- laying on side

        float q = length( p.xz );
        return dot( c.xy, vec2( q, p.y ) );

        // Alt. cone formula given in: ???
        //vec2 q = vec2( length( p.xz ), p.y );
        //float d1 = -p.y - c.z;
        //float d2 = max( dot(q,c.xy), p.y );
        //return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.0);
    }

    // ========================================
    float sdCylinder( vec3 p, vec3 c ) {
        return length(p.xz - c.xy) - c.z;
    }

    // n.xyz = point on plane
    // n.w   = distance to plane
    // Note: N must be normalized!
    // ========================================
    float sdPlane( vec3 p, vec4 n ) {
        return dot( p, n.xyz ) + n.w;
    }

    // 4 sided pyramid
    // h.x = base X
    // h.y = height
    // h.z = base Z (usually same as h.x)
    // ========================================
    float sdPyramid4( vec3 p, vec3 h ) {
        p.xz = abs(p.xz);                   // Symmetrical about XY and ZY
        vec3 n = normalize(h);
        return sdPlane(p, vec4( n, 0.0 ) ); // cut off bottom
    }

    // ========================================
    float sdSphere( vec3 p, float r ) {
        return length(p) - r;
    }

    // ========================================
    float sdSphere2( vec3 p, float r ) {
        return abs(length(p) - r);
    }

    // ========================================
    float sdTorus( vec3 p, vec2 t ) {
        vec2 q = vec2(length(p.xy) - t.x, p.z);
        return length(q) - t.y;
    }

    // TODO: document/derive magic number 0.866025
    // ========================================
    float sdTriPrism( vec3 p, vec2 h ) {
        vec3 q = abs(p);
        return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
    }
// __ Distance Operations _____________________________________

// Basic
    // Op Union
    // ========================================
    float opU( float d1, float d2 ) {
        return min( d1, d2 );
    }

    // Op Subtraction
    // ========================================
    float opS( float a, float b ) {
        return max( -b, a ); // BUG in iq's docs: -a, b
    }

    // Op Intersection
    // ========================================
    float opI( float a, float b ) {
        return max( a, b );
    }

// Advanced
    // ========================================
    float opBlend( float a, float b, float k ) {
        return sMin( a, b, k );
    }

    // a angle
    // ========================================
    float displacement( vec3 p, float a ) {
        return sin(a*p.x)*sin(a*p.y)*sin(a*p.z); // NOTE: Replace with your own!
    }

    // ========================================
    float opDisplace( vec3 p, float d1, float d2 ) {
        return d1 + d2;
    }

    // Op Union Translated
    // ========================================
    vec4 opUt( vec4 a, vec4 b, float fts ){
        vec4 vScaled = vec4(b.x * (fts * 2.0 - 1.0), b.yzw);
        return mix(a, vScaled, step(vScaled.x, a.x) * step(0.0, fts));
    }


// __ Domain Operations _______________________________________

// NOTE: iq originally inlined the primitive inside the Domain operations. :-(
// This implied that you would have needed to provide 
// a primitive with one of the sd*() functions above
// since we can't have a generic pointer to a function!
// However we have moved them back out to the caller
// for clarity and flexibility without general loss of precision.

// Basic

    // Op Repetition
    // ========================================
    vec3 opRep( vec3 p, vec3 spacing ) {
        return mod(p,spacing) - 0.5*spacing;
    }

// Deformations

    // Op Twist X
    // ========================================
    vec3 opTwistX( vec3 p, float angle ) {
        mat2 m = Rot2( angle * p.x );
        return   vec3( m*p.yz, p.x );
    }

    // Op Twist Y
    // ========================================
    vec3 opTwistY( vec3 p, float angle ) {
#if 0 // original
        float c = cos( angle * p.y );
        float s = sin( angle * p.y );
        mat2  m = mat2( c, -s, s, c );
        vec3  q = vec3( m*p.xz, p.y );
        // return primitive(q); // BUG in iq's docs, should be: return q
        return q;
#else // cleaned up
        mat2 m = Rot2( angle * p.y );
        return   vec3( m*p.xz, p.y );
#endif
    }

    // Op Twist Z
    // ========================================
    vec3 opTwistZ( vec3 p, float angle ) {
        mat2 m = Rot2( angle * p.z );
        return   vec3( m*p.xy, p.z );
    }

    // iq's bend X
    // ========================================
    vec3 opCheapBend( vec3 p, float angle ) {
#if 0 // original // broken :-(
        float c = cos( angle * p.y );
        float s = sin( angle * p.y );
        mat2  m = mat2( c, -s, s, c );
        vec3  q = vec3( m*p.xy, p.z ); // BUG in iq's docs, should be: p.yx
#else
        mat2  m = Rot2( angle * p.y );
        vec3  q = vec3( m*p.yx, p.z );
#endif
        return q;
    }

    // Op Cheap Bend X
    // ========================================
    vec3 opBendX( vec3 p, float angle ) {
        mat2 m = Rot2( angle * p.y );
        return   vec3( m*p.yx, p.z );
    }

    // Op Cheap Bend Y
    // ========================================
    vec3 opBendY( vec3 p, float angle ) {
        mat2 m = Rot2( angle * p.z );
        return   vec3( m*p.zy, p.x );
    }

    // Op Cheap Bend Z
    // ========================================
    vec3 opBendZ( vec3 p, float angle ) {
        mat2 m = Rot2( angle * p.x );
        return   vec3( m*p.xz, p.y );
    }

    // d = distance to move
    // ========================================
    vec3 opTrans( vec3 p, vec3 d ) {
        return p - d;
    }

    // Note: m must already be inverted!
    // TODO: invert(m) transpose(m)
    // Op Rotation / Translation
    // ========================================
    vec3 opTx( vec3 p, mat4 m ) {   // BUG in iq's docs, should be q
        return (transpose(m)*vec4(p,1.0)).xyz;
    }

    // Op Scale
    // ========================================
    float opScale( vec3 p, float s ) {
        return sdBox( p/s, vec3(1.2,0.2,1.0), 0.01 ) * s; // TODO: FIXME: NOTE: replace with primative sd*()
    }


/******************************
 Start here
******************************/

const float EPSILON=0.001;
const int STEPS=100;
const float MAX_DIST=30.;

//Pientä säätöä studiossa :D

float distFunc(vec3 p, out float material, float t)
{
    vec3 op = p;
    p+=vec3(sin(iGlobalTime)*2., 0.,3.);
    float radius = 0.9;
//    vec3 rep = opRep(p, vec3(3.1, 3.1, 3.1));
//    p = mix(rep, p, abs(cos(iGlobalTime*0.3)*0.2));
    p = opTwistZ(p , sin(iGlobalTime*0.4)*0.4+0.0);
    //p = opTwistY(p, cos(iGlobalTime*3.)*1.);
    float height = -2.5;
    float mp=0.9+sin(iGlobalTime)*0.4;
    float tt = iGlobalTime*3.;
    float ball = sdSphere(p + vec3(sin(tt)*4., cos(tt)*mp+height, .5), radius+sin(tt)*0.4);
    ball = sMin( ball, sdSphere(p +vec3(sin(tt+0.2)*4., cos(tt+.1)*mp+height, 1.), radius+sin(tt+.4)*0.4),1.5);
   	ball = sMin( ball, sdSphere(p +vec3(sin(tt+0.4)*4., cos(tt+.2)*mp+height, 1.5),radius+sin(tt+.8)*0.4),2.1);
   	ball = sMin( ball, sdSphere(p +vec3(sin(tt+0.6)*4., cos(tt+.3)*mp+height, 2.), radius+sin(tt+1.2)*0.4),2.1);
   	ball = sMin( ball, sdSphere(p +vec3(sin(tt+0.8)*4., cos(tt+.4)*mp+height, 2.5),radius+sin(tt+1.6)*0.4),2.1);
  	ball = sMin( ball, sdSphere(p +vec3(sin(tt+1.0)*4., cos(tt+.5)*mp+height, 3.), radius+sin(tt+2.)*0.4),2.1);
    
    
    
   	float ground = sdPlane(op, vec4(0, 1., 0.,2.));
    
    vec3 pyrp = op;
    pyrp -= vec3(0., 0., iGlobalTime*5.);
    pyrp = opRep(pyrp, vec3(4.,0., 4.));
    float pyramids = sdPyramid4(pyrp+vec3(0.,0.,0), vec3(0.1));
	
//    ball = 0.;
    
    
    material = ball <ground?0.:1.;
    material = pyramids < material ? 2. : material;
    float dist = opU(ball, ground);
    dist = opU(pyramids, dist);
    
    if(t < 0.4)
        dist = 0.4;
    return dist;
}

vec3 calcLight(vec3 p, float material, vec3 rd, float t)
{
    vec2 eps = vec2(0.0, EPSILON);
    float m = 0.;
    float d1 = distFunc(p + eps.yxx,m,t);
    float d2 = distFunc(p - eps.yxx,m,t);
	vec3 normal = normalize(vec3(
    d1 - d2,
    distFunc(p + eps.xyx,m,t) - distFunc(p - eps.xyx,m,t),
    distFunc(p + eps.xxy,m,t) - distFunc(p - eps.xxy,m,t)));
    vec3 diffuse = vec3(max(0.0, dot(-rd, normal)));
    
    if(material < 0.5)
    {
     //balls   
        diffuse*= vec3(1.7, 1.5,1.8);//*abs(sin(t*24.));
    }
    else if(material < 1.5)
    {
      //ground
        diffuse *= abs(sin(t*40.+iGlobalTime*3.));
    }
    else if(material < 2.5)
    {
     	//pyramids
        diffuse*=vec3(0., 0., 1.);
    }
    
    vec3 specular = pow(diffuse, vec3(32.0));
    
	return vec3(diffuse + specular);
}

vec3 setCamera( in vec3 eye, in vec3 target, in vec2 uv )
{
    float fov = 1.0;
	vec3 forward = normalize( target - eye );
    vec3 up = normalize( cross(forward,vec3(0.0,1.0,0.0) ) );
    vec3 right = normalize( cross(up,forward));
	vec3 raydir = normalize( uv.x*up + uv.y*right + fov*forward );
	return raydir;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    //Map uv to -1..1 & fix aspect
    uv*=2.;
    uv-=1.;
    uv.x*=iResolution.x/iResolution.y;
    float time = iGlobalTime;
    
    vec3 eye = vec3(sin(time)*.1,cos(time*1.)+.5,3.);
//    eye = vec3(0., 1., 1.);
    vec3 lookAt =  vec3(cos(time)*3.,cos(time*3.2)*0.3-1.6,sin(time));    
    lookAt = -eye;
    lookAt = vec3(0.);
    vec3 rayDir = setCamera(eye, lookAt, uv);
    
    vec3 color = vec3(0.);
    
    //Distance counter
    float t = 0.;
    vec3 bg = vec3(sin(time)*0.2+1.2, sin(time+0.4)*0.2+1.2, 1.2);
    bg = vec3(1.1,0.8,0.4);
    bg*=sin((uv.y+sin(time+uv.x))*20.)*0.3+1.;
    //March
    for(int i =0; i<STEPS;i++){
        vec3 p = rayDir*t+eye;
        float material = 0.;
        float dist = distFunc(p,material,t);
        
        if(dist < EPSILON )
        {
         	color = calcLight(p,material,rayDir, t);   
            color =mix(color, bg,max(0.,(t-20.)/(MAX_DIST-20.)));
            break;
        }
        if(t > MAX_DIST)
        {
         	color = bg;
         //   color = vec3(1.);
//            color *= mod(uv.x*uv.y+time*0.2, 0.06)+0.9; 
  //          color *= mod(uv.y+time*0.2, 0.06)+0.9; 
            break;
        }
            
        t += dist;
    }
	fragColor = vec4(color,1.0);
}

//KOODI EI ILMEISESTI NÄY? TEEEN REFRESHIN. HETKI
// from https://www.shadertoy.com/view/Xs3SWj

#define PI 3.14159

vec2 cInverse(vec2 a) { return	vec2(a.x,-a.y)/dot(a,a); }
vec2 cMul(vec2 a, vec2 b) {	return vec2( a.x*b.x -  a.y*b.y,a.x*b.y + a.y * b.x); }
vec2 cDiv(vec2 a, vec2 b) {	return cMul( a,cInverse(b)); }
vec2 cExp(vec2 z) {	return vec2(exp(z.x) * cos(z.y), exp(z.x) * sin(z.y)); }
vec2 cLog(vec2 a) {	float b =  atan(a.y,a.x); if (b>0.0) b-=2.0*PI;return vec2(log(length(a)),b);}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  fragCoord = (fragCoord - iResolution.xy/2.)/iResolution.y;
    float r1 = 0.1, r2 = 1.0,
        scale = log(r2/r1),angle = atan(scale/(2.0*PI));
    // Droste transform here
    fragCoord = cLog(fragCoord);
    fragCoord.y -= iGlobalTime/2.;
    fragCoord = cDiv(fragCoord, cExp(vec2(0,angle))*cos(angle)); // Twist!
    fragCoord.x = mod(fragCoord.x-iGlobalTime,scale);
    fragCoord = cExp(fragCoord)*r1;
    // Drawing time.
    fragCoord = sin(fragCoord*25.)*3.;
    fragColor = vec4(fragCoord.x*fragCoord.y);
}

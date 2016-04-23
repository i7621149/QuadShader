// from https://www.shadertoy.com/view/lsdXRs

//Ethan Alexander Shulman 2016
//Thanks to FabriceNeyret2 for optimization tips
void mainImage(out vec4 fragColor, vec2 fragCoord)
{
    vec3 p,q;
    float d, a=0.;

    fragColor = vec4(0.);
    for (int l = 0; l < 64; l++) {
        vec2 v = sin(a*.1 + vec2(0,1.6));
        q = normalize(vec3((fragCoord/iResolution.xy-.5)*mat2(v,-v.y,v.x),1));
        p = cos(iDate.w + q*a);
        d = max(0.05, sqrt(p.x*p.x+p.y*p.z+p.z*p.z));
        a += d*.5;
        fragColor.xyz += d*cos(q)*.01;
    }
}

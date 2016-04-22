// from https://www.shadertoy.com/view/4llGDj

// uses tex10

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy-.5;
	uv.x*=iResolution.x/iResolution.y;
	float l=length(uv);
	float a=iGlobalTime*.5;
	uv=mat2(cos(a),sin(a),-sin(a),cos(a))*uv;
	float r=texture2D(iChannel0,vec2(atan(uv.x,uv.y)*.05)).x;
	r=max(r,texture2D(iChannel0,vec2(pow(l,.25)*.2-iGlobalTime*.02)).x);
	float col=pow(r*(.35+l*.7),2.5);
	fragColor = vec4(col*1.5);
}

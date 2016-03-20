//------------------------------------------------------------//
//-----------------------DEFAULT-SHADER-----------------------//
//------------------------------------------------------------//
//                                                            //
//             from https://www.shadertoy.com/new             //
//    This outputs a colour gradient that varies with time    //
//                                                            //
//    Most shaders written in Shadertoy format should work    //
//                                                            //
//------------------------------------------------------------//

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 uv = fragCoord.xy / iResolution.xy;
  fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime),1.0);
}

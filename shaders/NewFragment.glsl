void main()
{
  vec2 uv = fragCoord.xy / iResolution.xy;
  vec4 a = texture(iChannel0, uv);
  vec4 b = texture(iChannel1, uv);
  vec4 c = texture(iChannel2, uv);
  vec4 d = texture(iChannel3, uv);

  vec4 upper = mix(a, b, smoothstep(0.48, 0.52, uv.x));
  vec4 lower = mix(c, d, smoothstep(0.48, 0.52, uv.x));

  fragColor = mix(upper, lower, smoothstep(0.48, 0.52, uv.y));

}

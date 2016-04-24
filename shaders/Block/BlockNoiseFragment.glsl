void main()
{
  vec3 lightDirection = normalize(vec3(-0.7, 1.0, 0.7));

  float normalOffset = length(fragNormal - lightDirection);

  fragColor = vec4(1.0, 1.0, 0.0, 1.0);

  if(iMatID == 4) // wall
  {
    fragColor = vec4(0.0, sin(iGlobalTime), 0.0, 1.0);
  }
  else if(iMatID == 5) // floor
  {
    vec2 uv = fragUV + vec2(iGlobalTime, iGlobalTime*iGlobalTime);
    fragColor = texture(iChannel0, uv*10);
  }

  fragColor.rg *= clamp(1.0-normalOffset+0.8, 0.0, 1.0);
  fragColor.b *= clamp(1.0-normalOffset+0.9, 0.0, 1.0);
}

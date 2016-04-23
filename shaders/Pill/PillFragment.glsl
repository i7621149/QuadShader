void main()
{
  vec3 lightDirection = normalize(vec3(-0.7, 1.0, 0.7));

  float normalOffset = length(fragNormal - lightDirection);

  fragColor = texture(iChannel0, fragUV);
  if(fragColor.r < 0.5)
  {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else
  {
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  }

  fragColor.rg *= clamp(1.0-normalOffset + 0.8, 0.0, 1.0);
  fragColor.b *= clamp(1.0-normalOffset + 0.9, 0.0, 1.0);

  if(normalOffset < 0.3)
  {
    fragColor = vec4(1.0);
  }
}

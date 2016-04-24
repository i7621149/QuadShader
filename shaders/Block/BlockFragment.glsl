void main()
{
  vec3 lightDirection = normalize(vec3(-0.7, 1.0, 0.7));

  float normalOffset = length(fragNormal - lightDirection);

  fragColor = vec4(1.0, 1.0, 0.0, 1.0);

  if(iMatID == 4) // wall
  {

  }
  else if(iMatID == 5) // floor
  {
    fragColor = texture(iChannel0, fragUV*10);
    fragColor = (fragColor + fragColor.rrrr) / 2.0;
  }

  fragColor.rg *= clamp(1.0-normalOffset+0.8, 0.0, 1.0);
  fragColor.b *= clamp(1.0-normalOffset+0.9, 0.0, 1.0);
}

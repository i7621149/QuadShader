void main()
{
  vec3 lightDirection = normalize(vec3(-0.7, 1.0, 0.7));

  float normalOffset = length(fragNormal - lightDirection);

  fragColor = texture(iChannel0, fragUV);

  if(fragColor.r > 0.8 && fragColor.g < 0.2)
  {
    fragColor = texture(iChannel1, fragUV/2.0);
    if(iMatID == 1) // player1
    {
      fragColor.r += 1.0;
      fragColor.g += 0.5;
      fragColor *= 0.7;
    }
    else if(iMatID == 2) // player2
    {
      fragColor += vec4(0.5);
      fragColor *= 0.8;
    }
  }

  fragColor.rg *= clamp(1.0-normalOffset+0.8, 0.0, 1.0);
  fragColor.b *= clamp(1.0-normalOffset+0.9, 0.0, 1.0);
}

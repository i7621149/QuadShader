void main()
{
  fragColor = texture(iChannel0, fragUV);

  if(fragColor.r > 0.8 && fragColor.g < 0.2)
  {
    fragColor = texture(iChannel1, fragUV/2.0);
    if(iMatID == 1)
    {
      fragColor.r += 0.5;
      fragColor.g += 0.3;
      fragColor *= 0.7;
    }
    else if(iMatID == 2)
    {
      fragColor += vec4(0.5);
      fragColor *= 0.8;
    }
  }
}

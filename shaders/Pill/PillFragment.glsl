void main()
{
  fragColor = texture(iChannel0, fragUV);
  if(fragColor.r < 0.5)
  {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else
  {
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  }
}

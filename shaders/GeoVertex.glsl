// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  fragPosition = inPosition;
  fragNormal = inNormal;
  fragUV = inUV;
  gl_Position = MVP * vec4(inPosition, 1.0);
}

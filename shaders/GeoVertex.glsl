// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  setFragVariables();
  gl_Position = MVP * vec4(inPosition, 1.0);
}

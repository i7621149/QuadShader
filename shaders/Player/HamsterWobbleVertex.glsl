// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  setFragVariables();

  float scale = 1.5*abs(sin(vGlobalTime)) + 0.1;
  scale *= scale;
  vec3 pos = inPosition * scale;

  gl_Position = MVP * vec4(pos, 1.0);
}

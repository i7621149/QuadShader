// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  setFragVariables();

  float scale = vGlobalTime / 30.0 + 0.5;
  vec3 pos = inPosition * scale;

  gl_Position = MVP * vec4(pos, 1.0);
}

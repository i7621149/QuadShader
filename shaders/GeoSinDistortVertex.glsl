// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  vec4 vertPos = vec4(inPosition, 1.0);

  vertPos.y += 0.2*sin((M*vertPos).x*0.3 + 5*vGlobalTime);

  setFragVariables();
  gl_Position = MVP * vertPos;
}

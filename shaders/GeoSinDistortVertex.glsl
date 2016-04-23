// simple quad shader, minus parts included in BaseVertex.glsl
void main()
{
  setFragVariables();

  float scale = vGlobalTime / 12.0;
  vec4 vertPos = vec4(inPosition, 1.0);

  float xDistort = 0.5*sin(0.2*vGlobalTime);
  float zDistort = 0.5*cos(0.2*vGlobalTime);

  //vertPos.y += 0.2*sin((translateMat*vertPos).x*0.3 + 5*vGlobalTime);

  gl_Position = MVP * vertPos;
  gl_Position.y += scale*sin((M*vertPos).x*xDistort + (M*vertPos).z*zDistort);
}

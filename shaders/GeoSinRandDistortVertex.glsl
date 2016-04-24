// random looking enough for distorting the character and pill models
// not really particularly random in reality, depends on time and vert position
void main()
{
  float scale = vGlobalTime / 120.0;

  setFragVariables();
  int rseed = int((inPosition.x + inPosition.y + inPosition.z + vGlobalTime) * 1000.0);

  vec3 displacement;

  int mult = 25173;
  int inc = 13849;
  int mod = 65536;

  displacement.x = (mult*rseed+inc) % mod;
  displacement.y = (mult*int(displacement.x)+inc) % mod;
  displacement.z = (mult*int(displacement.y)+inc) % mod;

  displacement -= mult;
  displacement /= mult;
  displacement *= scale;

  scale = vGlobalTime / 12.0;
  vec4 vertPos = vec4(inPosition + displacement, 1.0);

  float xDistort = 0.5*sin(0.2*vGlobalTime);
  float zDistort = 0.5*cos(0.2*vGlobalTime);

  //vertPos.y += 0.2*sin((translateMat*vertPos).x*0.3 + 5*vGlobalTime);

  gl_Position = MVP * vertPos;
  gl_Position.y += (scale)*sin((M*vertPos).x*xDistort + (M*vertPos).z*zDistort);
}

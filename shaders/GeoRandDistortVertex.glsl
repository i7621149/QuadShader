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

  gl_Position = MVP * vec4(inPosition + displacement, 1.0);
}

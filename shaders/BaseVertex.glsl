#version 410 core
//very simple shader, can be changed
layout (location = 0) in vec3  inPosition;

void main()
{
  gl_Position = vec4(inPosition, 1.0);
}

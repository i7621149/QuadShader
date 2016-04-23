#version 410 core

// positions in
layout (location = 0) in vec3  inPosition;

// normals in
layout (location = 2) in vec3 inNormal;

// uvs in
layout (location = 1) in vec2 inUV;

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform mat4 M;

uniform int matID;

// inputs are uniforms set in correct format for shadertoy to interface with
uniform vec3 vResolution;
uniform float vGlobalTime;
uniform float vTimeDelta;

// currently frame rate is locked to 60fps, probably should be variable or something
// incremented every time the frame renders, in paintGL
uniform int vFrame;

// channel time and resolution are not working particularly well
//uniform float iChannelTime[4];
//uniform float iChannelResolution[4];
uniform vec4 vMouse;

// date works correctly, using QDateTime variable
uniform vec4 vDate;

// to do with audio?
//uniform float vSampleRate;
//----------------------------------------------------------------------------------------------------------------//

uniform int vMatID;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragUV;

void setFragVariables()
{
  fragPosition = inPosition;
  fragNormal = normalize(normalMatrix*inNormal);
  fragUV = inUV;
}

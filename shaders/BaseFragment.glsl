#version 410 core

// redefine mainImage funciton from shadertoy
#define mainImage(a,b) main()

// ouput
out vec4 fragColor;

// SHADERTOY INTERFACE - INPUTS
//----------------------------------------------------------------------------------------------------------------//
// define fragCoord globally, simple solution to reatin compatibility with shadertoy
vec2 fragCoord = gl_FragCoord.xy;

// inputs are uniforms set in correct format for shadertoy to interface with
uniform vec3 iResolution;
uniform float iGlobalTime;
uniform float iTimeDelta;

// currently frame rate is locked to 60fps, probably should be variable or something
// incremented every time the frame renders, in paintGL
uniform int iFrame;

// channel time and resolution are not working particularly well
// uniform float iChannelTime[4];
// uniform float iChannelResolution[4];
uniform vec4 iMouse;

// need to set up multiple channels, maybe they ought to be flexible, although this could be done outside of shader
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

// date works correctly, using QDateTime variable
uniform vec4 iDate;

// to do with audio?
// uniform float iSampleRate;
//----------------------------------------------------------------------------------------------------------------//

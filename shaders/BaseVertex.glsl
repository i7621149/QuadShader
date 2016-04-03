#version 410 core

// positions in
layout (location = 0) in vec3  inPosition;

// normals in
layout (location = 2) in vec3 inNormal;

// uvs in
layout (location = 1) in vec2 inUV;

uniform mat4 MVP;

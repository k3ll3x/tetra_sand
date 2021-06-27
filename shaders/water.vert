#version 330

uniform float time;
uniform vec2 resolution;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform mat4 mvp;

mat4 nmvp = mvp;

out vec2 coord;

in vec3 VertexPosition;
in vec2 VertexCoord;

in vec3 VertexColor;

void main(){
	vec3 nvp = VertexPosition;
	gl_Position = nmvp * vec4(nvp, 1.0f);
	coord = VertexColor;
}

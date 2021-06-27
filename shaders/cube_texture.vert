#version 330

uniform float time;
uniform vec2 resolution;
uniform mat4 mvp;

mat4 nmvp = mvp;

out vec2 coord;

in vec3 VertexPosition;
in vec2 VertexCoord;

void main(){
	gl_Position = nmvp * vec4(VertexPosition, 1.0f);
	coord = VertexCoord;
}

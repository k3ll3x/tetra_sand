#version 330

uniform float time;
uniform vec2 resolution;
uniform mat4 mvp;

//tmp
uniform vec4 mvp_v1;
uniform vec4 mvp_v2;
uniform vec4 mvp_v3;
uniform vec4 mvp_v4;
mat4 nmvp = mat4(mvp_v1,mvp_v2,mvp_v3,mvp_v4);

out vec3 color;

in vec3 VertexPosition;
in vec3 VertexColor;

void main(){
	gl_Position = nmvp * vec4(VertexPosition, 1.0f);
	color = VertexColor;
}


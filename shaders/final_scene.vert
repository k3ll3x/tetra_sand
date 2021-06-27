#version 330

uniform float time;
uniform vec2 resolution;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvp;

mat4 nmvp = mvp;

out vec2 coord;
out vec3 pos;
out vec3 n;

out vec3 VP;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexCoord;

void main(){
	vec3 nvp = VertexPosition;
	pos = vec3(modelMatrix * vec4(VertexPosition,1.0f));
	n = normalMatrix * VertexNormal;
	n = normalize(n);
	gl_Position = nmvp * vec4(nvp, 1.0f);
	coord = VertexCoord;
}

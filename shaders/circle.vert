#version 330

uniform vec2 iResolution;
uniform float iTime;

in vec3 VertexPosition;
in vec3 VertexColor;

out vec3 color;

void main(){
	gl_Position = vec4(VertexPosition, 1.0f);
	color = VertexColor;
}

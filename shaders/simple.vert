#version 330

in vec3 VertexPosition;
in vec4 InColor;
out vec4 OutColor;

void main(){
	gl_Position = vec4(VertexPosition, 1.0f);
	OutColor = InColor;
}

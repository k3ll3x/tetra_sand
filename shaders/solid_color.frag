#version 330

out vec4 FragColor;

in vec4 InterpolatedColor;

void main(){
	FragColor = InterpolatedColor;//vec4(1.0f, 0.0f , 0.0f, 1.0f);
}

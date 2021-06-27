#version 330

//texture
uniform sampler2D texture;

uniform float time;

out vec4 FragColor;

in vec2 coord;

void main(){
	FragColor = texture2D(texture,coord);
	//FragColor = vec4(coord,0.0f,1.0f);

}

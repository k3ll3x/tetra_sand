#version 330

out vec4 FragColor;

void main(){
	FragColor = vec4(gl_FragColor.x, gl_FragColor.y, gl_FragColor.z, 1.0f);
}

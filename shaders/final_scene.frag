#version 330

//texture
uniform sampler2D texture;
uniform vec2 resolution;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;
uniform mat4 mvp;

uniform float time;

out vec4 FragColor;

in vec2 coord;
in vec3 pos;
in vec3 n;

in vec3 VP;

void main(){
	vec3 ambiental =  LightColor * 0.05f;
	vec3 la = LightPosition - pos;
	la = normalize(la);
	float thetaX = dot(la, n);
	thetaX = clamp(thetaX, 0.8f, 1.0f);
	vec3 diffuse = thetaX * LightColor;

	vec3 ref = reflect(-la, n);
	vec3 lv = CameraPosition - pos;
	lv = normalize(lv);
	float thetaY = clamp(dot(ref, lv), 0.0f, 1.0f);
	thetaY = pow(thetaY, 0.5f);
	vec3 spec = thetaY * LightColor;

	vec3 phong = (ambiental + spec + diffuse);

	FragColor = texture2D(texture,coord) * vec4(phong, 1.0f);
	//FragColor = vec4(coord,0.0f,1.0f);
	//FragColor = vec4(color,1.0f);

}


#version 330

uniform vec2 iResolution;
uniform float iTime;

out vec4 FragColor;

in vec3 color;
//in vec4 gl_FragCoord;

void main(){
	vec2 p = gl_FragCoord.xy / iResolution.xy;
	float d = distance(p,vec2(0.5));
	if(d < 0.25f){
		discard;
	}

	FragColor = vec4(color, 1.0f);
}

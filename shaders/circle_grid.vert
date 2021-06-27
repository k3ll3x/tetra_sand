#version 330

uniform float time;

out vec4 InterpolatedColor;

vec2 circle(float circleId,float grid,float distance){
    float y = mod(gl_VertexID+1.0,2.0);
    float x = floor(gl_VertexID/2.0);
    float angle = x / 20.0 * radians(360.0);

    float r = 2.0 - y;

    x = r*cos(angle);
    y = r*sin(angle);
  
    x += mod(circleId,grid)*distance;
    y += floor(circleId/grid)*distance;
 
    
    float u = x;//grid*3.0;// / (grid);
    float v = y;//grid*3.0;// / (grid);

    vec2 xy = vec2(u, v) * 0.025;
    return xy;
}

void main(){
    float vertexPerCircle = 42.0;
    float circleId = floor(gl_VertexID/vertexPerCircle);
    float grid = 90.0;
    float distance = 4.5;

    vec2 xy = circle(circleId,grid,distance);
  
	float xOffset = cos(time + xy.y * 0.2f) * 0.2f;
	float yOffset = cos(time + xy.x * 0.3f) * 0.3f;

	float ux = xy.x * 2.0f - 1.0f + xOffset;
	float uy = xy.y * 2.0f - 1.0f + yOffset;

    gl_Position = vec4(ux-1.0,uy-1.0, 0.0, 1.0);
    InterpolatedColor = vec4(sin(time*xy.x), cos(time*xy.y), cos(time*xy.x*xy.y), 1.0);//vec4(1.0,0.0,0.0,1.0);

    gl_PointSize = 10.0;
}

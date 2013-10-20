layout(location = 0) in vec2 aTexCoord;
layout(location = 1) in vec2 aPos;
layout(location = 2) in vec2 aVel;
layout(location = 3) in vec2 aAccel;
layout(location = 4) in float aStartTime;
layout(location = 5) in vec3 aColor;

uniform mat4 uProjectionMatrix;
uniform float uCurrentTime;

out vec2 vTexCoord;
out vec3 vColor;

void main()
{
	vColor = aColor;

	vTexCoord = aTexCoord;
	//world_vertex += 0.001*noise_fac;
	gl_Position = uProjectionMatrix*(vec4(aPos, -0.5, 1.0));
}

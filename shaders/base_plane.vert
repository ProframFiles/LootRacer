layout(location = 0) in vec2 aTexCoord;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aVertex;
layout(location = 3) in vec4 aTangent;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uNormalMatrix;
uniform vec4 uPlayerPosition;
uniform vec3 uLight0;
#define NUM_LIGHTS 3
uniform vec4 uOrangeLights[NUM_LIGHTS];
uniform vec4 uBlueLights[NUM_LIGHTS];
uniform vec4 uPointerLoc;

out vec2 vTexCoord;
out vec3 vWorldVertex;
out vec3 vTangent;
out vec3 vNormal;
out vec3 vLightPos;
out vec4 vOrangeLights[NUM_LIGHTS];
out vec4 vBlueLights[NUM_LIGHTS];
out vec4 vPointerLoc;
out vec3 vPlayerCoord;

vec4 GetLight()
{
	return vec4 (0.0, 150.0,0.0,1.0);
}
vec3 NormalTransform(vec3 v){
	return (uNormalMatrix*(vec4(v, 0))).xyz;
}

vec3 NormalTransform(vec4 v){
	return NormalTransform(v.xyz);
}
vec3 X()
{
	return vec3(1.0, 0.0, 0.0);
}
vec3 BiTangent(in vec3 normal,in vec3 tangent,in float m)
{
	return normalize(m*cross(normal, tangent));
}

void main()
{
	for(int i= 0; i<NUM_LIGHTS ; i++){
		vBlueLights[i]= uBlueLights[i];
		vOrangeLights[i]= uOrangeLights[i];
	}
	

	float player_dist = length((uPlayerPosition.xyz-aVertex).xz);
	float height_factor = smoothstep(-4.0,0.3, -uPlayerPosition.w);
	vec3 adjusted_vertex = aVertex-vec3(0.0, height_factor*4.0*smoothstep(-15.0, 0.0, -player_dist), 0.0);
	vec4 world_vertex = uModelViewMatrix*(vec4(adjusted_vertex, 1.0));
	vPointerLoc = uPointerLoc;
	vLightPos = GetLight().xyz;
	vPlayerCoord = uPlayerPosition.xyz;
	vWorldVertex = adjusted_vertex;//world_vertex.xyz;
	vTangent = normalize(aTangent.xyz);
	vNormal = normalize(aNormal.xyz);
	vTexCoord = aTexCoord.xy;
	//world_vertex += 0.001*noise_fac;
	gl_Position = uProjectionMatrix*(world_vertex);
}

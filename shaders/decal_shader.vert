layout(location = 0) in vec2 aTexCoord;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aVertex;
layout(location = 3) in vec4 aTangent;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uNormalMatrix;
uniform vec3 uPlayerPosition;
uniform vec3 uLight0;

out vec2 vTexCoord;
out vec3 vWorldVertex;
out vec3 vTangent;
out vec3 vBiTangent;
out vec3 vNormal;

uniform float uCurrentTime;

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
	return normalize(cross(normal, tangent));
}

void main()
{
	vec4 world_vertex = uModelViewMatrix*(vec4(aVertex, 1.0));
	vWorldVertex = world_vertex.xyz;
	vTangent = aTangent.xyz;
	vNormal = normalize(aNormal);
	vBiTangent = BiTangent(aNormal, aTangent.xyz, -aTangent.w);
	vTexCoord = aTexCoord.xy;
	//world_vertex += 0.001*noise_fac;
	gl_Position = uProjectionMatrix*(world_vertex);
}

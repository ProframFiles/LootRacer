in vec2 vTexCoord;
in vec3 vWorldVertex;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec3 vPlayerCoord;
in vec3 vLightPos;
out vec4 oFragColor;
uniform float uCurrentTime;
uniform int uNumLights;
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform mat4 uNormalMatrix;

vec3 HueToRGB(float hue)
{
	vec3 s = vec3(1.0, -1.0, -1.0);
	return sqrt(clamp(vec3(-1.0,2.0, 2.0)+s*abs(vec3(3.0,2.0, 4.0 )+vec3(-hue*6.0)), 0.0, 1.0));
}
vec3 NormalTransform(vec3 v){
	return (uNormalMatrix*(vec4(v, 0))).xyz;
}

vec4 DebugColor()
{
	return vec4(1.0, 0.0, 1.0, 1.0);
}
vec3 rotate_by_quat(vec3 v, vec4 q)
{
	return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w*v );
}
vec3 reverse_rotate_by_quat(vec3 v, vec4 q)
{
	return v + 2.0 * cross(q.xyz, cross(q.xyz, v) - q.w*v );
}
vec3 Screen(vec3 color, vec3 light )
{
	return 1.0 - (1.0 - color)*(1.0 - light);
}
vec3 Screen(vec3 color, float light)
{
	return Screen(color, vec3(light));
}
//vec3 ToTangentSpace(vec3 v){
//	return vec3(dot(vWorldTangent, v),dot(vWorldBiTangent, v), dot(vWorldNormal, v) );
//}

vec3 Overlay(vec3 color, vec3 light)
{
	vec3 ba = 2.0*light*color;
	vec3 s = sign(ba-color);
	return color+s*min(abs(ba-color), abs(2.0*Screen(color, light)-color));
}
vec3 Overlay(vec3 color, float light)
{
	return Overlay(color, vec3(light));
}
vec3 SoftLight(vec3 color, vec3 light)
{
	 return color*(2.0*light*(1.0-color) + color);
}
vec3 SoftLight(vec3 color, float light)
{
	 return SoftLight(color, vec3(light));
}
vec3 PhongLighting(vec3 n, vec3 to_light,vec3 to_eye,  vec3 color, vec3 light_color,float ambient,float ao, float ro)
{
	float l = length(to_light);
	float dot_factor = dot(n, to_light/l);
	vec3 ret = 0.0*color*ambient*ao;
	ret += 0.0*clamp(dot_factor*light_color*color, 0.0, 1.0);
	vec3 spec_light = reflect(-to_light, n);
	float spec_term = dot(normalize(to_eye), normalize(spec_light));
	ret += 0.0*clamp(ro*pow(spec_term, 20.0), 0.0, 1.0);
	return clamp(ret, 0.0, 1.0);
}	


void main()
{	
	vec3 worldTangent = NormalTransform(vTangent);
	vec3 worldNormal = NormalTransform(vNormal);
	vec3 worldBiTangent = NormalTransform(vBiTangent); 

	float t = sin(uCurrentTime*0.1);
	vec3 cat = 0.5+0.5*cos(vec3(vTexCoord.xy, 0.5));
	vec4 diffuse_map = texture2D(uTexture0, vTexCoord);
	vec4 normal_map = texture2D(uTexture1, vTexCoord);
	vec3 tex_normal = normalize(normal_map.xyz);
	vec4 light_map = texture2D(uTexture2, vTexCoord);

	vec3 v = vLightPos - vWorldVertex;
	vec3 e = vPlayerCoord - vWorldVertex;
	vec3 to_light = vec3(dot(worldTangent, v),dot(worldBiTangent, v), dot(worldNormal, v) );
	vec3 to_eye = vec3(dot(worldTangent, e),dot(worldBiTangent, e), dot(worldNormal, e) );
	oFragColor.xyz = PhongLighting(normalize(tex_normal), to_light,to_eye, diffuse_map.xyz, vec3(0.0),0.0, light_map.x, light_map.y);
	//oFragColor.xyz =  Overlay((0.5f*light_map.x+0.2)*diffuse_map.xyz,vec3(0.25+0.75*dot(tex_normal, normalize(to_light))));
	oFragColor.a = diffuse_map.a;
}

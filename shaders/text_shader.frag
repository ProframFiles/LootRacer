in vec2 vTexCoord;
in vec3 vColor;
out vec4 oFragColor;
uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

vec3 GetLight(){
	return vec3(15.0, 12.0, 30.0);
}

vec3 HueToRGB(float hue)
{
	vec3 s = vec3(1.0, -1.0, -1.0);
	return sqrt(clamp(vec3(-1.0,2.0, 2.0)+s*abs(vec3(3.0,2.0, 4.0 )+vec3(-hue*6.0)), 0.0, 1.0));
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

void main()
{	
	vec4 diffuse_map = texture2D(uTexture0, vTexCoord);
	vec3 profile_map = texture2D(uTexture1, vec2(abs(diffuse_map.r))).xyz;
	oFragColor.xyz = Overlay(vColor,profile_map.y*0.5);//Overlay(vColor, profile_map.y);
	//oFragColor.xyz = Overlay(vColor, oFragColor.xyz );
	oFragColor.a = profile_map.r;
}

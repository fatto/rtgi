in vec2 coordinates;
layout(location = 0) out vec4 colour;

layout (r32ui) uniform readonly uimage3D volume_texture;
uniform mat4 mvp_inverse;


vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

vec3 rayAABB_test(vec3 ray_origin, vec3 ray_dir, vec3 aabb_min, vec3 aabb_max)
{
	float t_min, t_max;

	vec3 inv_ray = 1.0 / ray_dir;
	vec3 t1 = (aabb_min - ray_origin) * inv_ray;
	vec3 t2 = (aabb_max - ray_origin) * inv_ray;

	vec3 tmin = min(t1, t2);
	vec3 tmax = max(t1, t2);

	t_min = max(tmin.x, max(tmin.y, tmin.z));
	t_max = min(min(99999.0, tmax.x), min(tmax.y, tmax.z));

	if(t_min < 0.0) t_min = 0.0;

	vec3 result;
	result.x = (t_max > t_min) ? 1.0 : 0.0;
	result.y = t_min;
	result.z = t_max;

	return result;
}

void main()
{
	const ivec3 tex_size = imageSize(volume_texture);
	vec2 coord = coordinates*vec2(0.5) + vec2(0.5);
	vec2 ray_coords = coordinates;

	vec4 ndc0 = vec4(ray_coords, -1.0, 1.0);
	vec4 ndc1 = vec4(ray_coords, -0.5, 1.0);
	mat4 mat_inv = inverse(mvp_inverse); // TODO inverse on c++ side :(
	vec4 world0 = mat_inv * ndc0;
	vec4 world1 = mat_inv * ndc1;
	world0 /= world0.w;
	world1 /= world1.w;

	vec3 ray_origin = world0.xyz;
	vec3 ray_dir = world1.xyz - world0.xyz;
	ray_dir = normalize(ray_dir);

	const float epsilon = 0.00001;
	if (abs(ray_dir.x) <= epsilon) ray_dir.x = epsilon * sign(ray_dir.x);
	if (abs(ray_dir.y) <= epsilon) ray_dir.y = epsilon * sign(ray_dir.y);
	if (abs(ray_dir.z) <= epsilon) ray_dir.z = epsilon * sign(ray_dir.z);

	vec3 inv_ray = 1.0 / ray_dir;

	vec4 final_colour = vec4(0.1f);
	vec3 result = rayAABB_test(ray_origin, ray_dir, vec3(0.0), imageSize(volume_texture));
	if(result.x != 0.0)
	{
		float t_min = result.y;
		float t_max = result.z;

		vec3 start_pos = ray_origin + ray_dir * t_min;
		vec3 voxel_pos = max(vec3(0.0), min(imageSize(volume_texture) - vec3(1.0), floor(start_pos)));

		while(all(greaterThanEqual(voxel_pos, vec3(0.0))) && all(lessThan(voxel_pos, imageSize(volume_texture))))
		{
			vec4 partial_colour = convRGBA8ToVec4(imageLoad(volume_texture, ivec3(voxel_pos)).r);

			if(partial_colour.a > 0.0)
			{
				colour += partial_colour;
				break;
			}

			vec3 t0 = (voxel_pos - start_pos) * inv_ray;
			vec3 t1 = (voxel_pos + vec3(1.0) - start_pos) * inv_ray;
			vec3 tmax = max(t0, t1);
			float t = min(tmax.x, min(tmax.y, tmax.z));
			if(tmax.x == t) voxel_pos.x += sign(ray_dir.x);
			else if(tmax.y == t) voxel_pos.y += sign(ray_dir.y);
			else voxel_pos.z += sign(ray_dir.z);
		}
	}

	ivec2 denorm_coord = ivec2(floor(tex_size.xy*coord));

	colour = vec4(0.0,0.0,0.0,1.0);
	//colour = vec4(x_coord, y_coord, tex_size.z,1.0)/ vec4(255);

	for(int pos = 0; pos < tex_size.z; ++pos)
	{
		colour += convRGBA8ToVec4(imageLoad(volume_texture, ivec3(denorm_coord, pos)).r);
		//colour += ivec4(x_coord, y_coord, pos, 0.0)/vec4(255);
	}
	//colour = vec4(vec2(denorm_coord)/vec2(128), 0.0, 1.0);
}
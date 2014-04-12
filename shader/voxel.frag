in VoxelData
{
	vec3 position;
	vec3 normal;
	vec3 colour;
	mat3 swizzle_inv;
	vec4 bbox;
	// vec2 uv;
} In;

//vec3 light_dir = normalize(vec3(-1,-1,-1)); // hardcoding for the win

layout (r32ui, location=0) coherent uniform uimage3D volume_texture;
layout (location = 0) out vec4 frag_colour;

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

void main()
{
	ivec2 viewport_size = imageSize(volume_texture).xy;
	vec2 bbox_min = floor((In.bbox.xy * 0.5 + 0.5) * viewport_size);
	vec2 bbox_max = ceil((In.bbox.zw * 0.5 + 0.5) * viewport_size);
	if (all(greaterThanEqual(gl_FragCoord.xy, bbox_min)) && all(lessThanEqual(gl_FragCoord.xy, bbox_max)))
	{
		vec3 coords = In.swizzle_inv * vec3(gl_FragCoord.xy, gl_FragCoord.z * viewport_size.x);
		//vec3 colour = In.colour*vec3(0.7) + (coords/vec3(imageSize(volume_texture))) * vec3(0.3);
		vec3 colour = In.colour;
		//vec3 coords = vec3(gl_FragCoord.xy, gl_FragCoord.z * viewport_size.x);
		//mageStore(volume_texture, ivec3(coords), vec4(In.color, 1.0));
		//imageStore(volume_texture, ivec3(coords), uvec4(convVec4ToRGBA8(vec4(In.colour, 1.0))));
		imageStore(volume_texture, ivec3(coords), uvec4(convVec4ToRGBA8(vec4(colour, 1.0))));
	}
	else
	{
		discard;
	}
}
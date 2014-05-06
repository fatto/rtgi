#extension GL_NV_shader_atomic_float : require

in VoxelData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	flat vec4 bbox;
	flat int axis;
} In;

layout(location = 0) out vec4 out_colour;
layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(binding = 0, offset = 0) uniform atomic_uint voxel_frag_count;

layout(binding = 0, rgb10_a2ui) uniform uimageBuffer voxel_position;
layout(binding = 1, rgba8ui) uniform uimageBuffer voxel_diffuse;
layout(binding = 2, rgba32f) uniform imageBuffer voxel_normal;

uniform float shininess;
uniform usampler2D colour_tex;
uniform sampler2D bump_tex;
uniform bool textured;
uniform bool bump;

uniform uint voxel_dim;
uniform bool store;


// layout(location = 0, r32ui) /*coherent*/ restrict uniform uimage3D volume_texture;

// uint convVec4ToRGBA8(vec4 val) {
//     return (uint(val.w) & 0x000000FF)   << 24U
//             |(uint(val.z) & 0x000000FF) << 16U
//             |(uint(val.y) & 0x000000FF) << 8U 
//             |(uint(val.x) & 0x000000FF);
// }

void main()
{
	// ivec2 viewport_size = ivec2(voxel_dim);
	vec2 bbox_min = In.bbox.xy;
	vec2 bbox_max = In.bbox.zw;

	if(any(lessThan(In.position.xy, bbox_min)) || any(greaterThan(In.position.xy, bbox_max)))
	{
		discard;
		// return;
	}

	uvec4 pos = uvec4(gl_FragCoord.xy, gl_FragCoord.z * voxel_dim, 0.0);
	uvec4 coord;
	if(In.axis == 1)
	{
		coord.x = voxel_dim - pos.z;
		coord.y = pos.y;
		coord.z = pos.x;
	}
	else if(In.axis == 2)
	{
		coord.x = pos.x;
		coord.y = voxel_dim - pos.z;
		coord.z = pos.y;
	}
	else
		coord = pos;

	uint idx = atomicCounterIncrement(voxel_frag_count);
	if(store)
	{
		uvec3 C;
		vec3 N;
		
		if(textured)
			C = texture(colour_tex, In.texcoord).rgb;
		else
		{
			// C = clamp(uvec3(2-In.axis, 3-In.axis, 4-In.axis), uvec3(0), uvec3(1)); // (1,1,1) x-axis, (0,1,1) y-axis, (0,0,1) z-axis
			if(In.axis == 1)
				C = uvec3(1, 0, 0);
			else if(In.axis == 2)
				C = uvec3(0, 1, 0);
			else
				C = uvec3(0, 0, 1);
		}
		if(bump)
			N = texture(bump_tex, In.texcoord).rgb;
		else
			N = In.normal;

		imageStore(voxel_position, int(idx), coord);
		imageStore(voxel_diffuse, int(idx), uvec4(C, 0));
		imageStore(voxel_normal, int(idx), vec4(N, 0));
	}
}
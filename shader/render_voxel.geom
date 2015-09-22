layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec4 position;
	uvec3 texcoord;
} In[3];

out FragmentData
{
	vec4 colour; // alpha used as counter
} Out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform uint voxel_dim;
uniform uint level;
uniform float half_cube;

layout(binding = 3, r32ui) uniform uimageBuffer octree_buffer;

layout(binding = 4, r32ui) uniform uimageBuffer octree_diffuse_r;
layout(binding = 5, r32ui) uniform uimageBuffer octree_diffuse_g;
layout(binding = 6, r32ui) uniform uimageBuffer octree_diffuse_b;
layout(binding = 7, r32ui) uniform uimageBuffer octree_diffuse_a;

bool emptyNode(in uvec3 location, out uint leaf)
{
	uvec3 umin = uvec3(0, 0, 0);
	uint node = 0;
	uvec3 offset;
	uint voxel_dimension = voxel_dim;
	bool empty = false;

	for(uint i = 0; i <= level; ++i)
	{
		leaf = node;
		node = imageLoad(octree_buffer, int(node)).r;
		if((node & 0x80000000) == 0)
		{
			empty = true;
			break;
		}
		else if( i == level)
		{
			break;
		}

		node &= 0x7fffffff; // get child pointer masking out flag bit
		if(node == 0)
		{
			empty = true;
			break;
		}
		voxel_dimension /= 2;

		offset = clamp(ivec3(1 + location - umin - voxel_dimension), 0, 1);
		node += offset.x + 4 * offset.y + 2 * offset.z;

		umin += voxel_dimension * offset;
	}
	return empty;
}

void main()
{
	uint leaf;
	vec4 colour;
	if(!emptyNode(In[0].texcoord, leaf))
	{
		colour = vec4(
			imageLoad(octree_diffuse_r, int(leaf)).r,
			imageLoad(octree_diffuse_g, int(leaf)).r,
			imageLoad(octree_diffuse_b, int(leaf)).r,
			imageLoad(octree_diffuse_a, int(leaf)).r);
	}
	else
	{
		return;
	}
	mat4 MVP = projection * view * model;

	Out.colour = colour;
	gl_Position = MVP * gl_in[0].gl_Position;
	EmitVertex();
	Out.colour = colour;
	gl_Position = MVP * gl_in[1].gl_Position;
	EmitVertex();
	Out.colour = colour;
	gl_Position = MVP * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}


// layout(points, invocations = 1) in;
// layout(triangle_strip, max_vertices = 24) out;

// in VertexData
// {
// 	vec4 position;
// 	uvec3 texcoord;
// } In[1];

// out FragmentData
// {
// 	vec4 colour; // alpha used as counter
// } Out;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
// uniform uint voxel_dim;
// uniform uint level;
// uniform float half_cube;

// layout(binding = 3, r32ui) uniform uimageBuffer octree_buffer;

// layout(binding = 4, r32ui) uniform uimageBuffer octree_diffuse_r;
// layout(binding = 5, r32ui) uniform uimageBuffer octree_diffuse_g;
// layout(binding = 6, r32ui) uniform uimageBuffer octree_diffuse_b;
// layout(binding = 7, r32ui) uniform uimageBuffer octree_diffuse_a;

// bool emptyNode(in uvec3 location, out uint leaf)
// {
// 	uvec3 umin = uvec3(0, 0, 0);
// 	uint node = 0;
// 	uvec3 offset;
// 	uint voxel_dimension = voxel_dim;
// 	bool empty = false;

// 	for(uint i = 0; i <= level; ++i)
// 	{
// 		leaf = node;
// 		node = imageLoad(octree_buffer, int(node)).r;
// 		if((node & 0x80000000) == 0)
// 		{
// 			empty = true;
// 			break;
// 		}
// 		else if( i == level)
// 		{
// 			break;
// 		}

// 		node &= 0x7fffffff; // get child pointer masking out flag bit
// 		if(node == 0)
// 		{
// 			empty = true;
// 			break;
// 		}
// 		voxel_dimension /= 2;

// 		offset = clamp(ivec3(1 + location - umin - voxel_dimension), 0, 1);
// 		node += offset.x + 4 * offset.y + 2 * offset.z;

// 		umin += voxel_dimension * offset;
// 	}
// 	return empty;
// }

// vec4[8] cube(in vec4 center_pos)
// {
// 	vec4[8] positions;
// 	positions[0] = center_pos + vec4(-half_cube, -half_cube, +half_cube, 0.0);
// 	positions[1] = center_pos + vec4(+half_cube, -half_cube, +half_cube, 0.0);
// 	positions[2] = center_pos + vec4(-half_cube, +half_cube, +half_cube, 0.0);
// 	positions[3] = center_pos + vec4(+half_cube, +half_cube, +half_cube, 0.0);
// 	positions[4] = center_pos + vec4(+half_cube, -half_cube, -half_cube, 0.0);
// 	positions[5] = center_pos + vec4(-half_cube, -half_cube, -half_cube, 0.0);
// 	positions[6] = center_pos + vec4(+half_cube, +half_cube, -half_cube, 0.0);
// 	positions[7] = center_pos + vec4(-half_cube, +half_cube, -half_cube, 0.0);
// 	return positions;
// }

// void main()
// {
// 	uint leaf;
// 	vec4 colour;
// 	if(!emptyNode(In[0].texcoord, leaf))
// 	{
// 		colour = vec4(
// 			imageLoad(octree_diffuse_r, int(leaf)).r,
// 			imageLoad(octree_diffuse_g, int(leaf)).r,
// 			imageLoad(octree_diffuse_b, int(leaf)).r,
// 			imageLoad(octree_diffuse_a, int(leaf)).r);
// 	}
// 	else
// 	{
// 		return;
// 	}
// 	mat4 MVP = projection * view * model;
// 	vec4[8] positions = cube(In[0].position);

// 	// + Z
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[0];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[1];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[2];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[3];
// 	EmitVertex();
// 	EndPrimitive();

// 	// - Z
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[4];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[5];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[6];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[7];
// 	EmitVertex();
// 	EndPrimitive();

// 	// + X
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[1];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[4];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[3];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[6];
// 	EmitVertex();
// 	EndPrimitive();

// 	// -X
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[5];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[0];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[7];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[2];
// 	EmitVertex();
// 	EndPrimitive();

// 	// + Y
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[2];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[3];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[7];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[6];
// 	EmitVertex();
// 	EndPrimitive();

// 	// -Y
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[5];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[4];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[0];
// 	EmitVertex();
// 	Out.colour = colour;
// 	gl_Position = MVP * positions[1];
// 	EmitVertex();
// 	EndPrimitive();
// }
#ifdef GL_AMD_shader_trinary_minmax
#extension GL_AMD_shader_trinary_minmax : require
#else
float max3(float a,float b,float c)
{
	return max(a, max(b,c));
}
float min3(float a,float b,float c)
{
	return min(a, min(b,c));
}
vec2 max3(vec2 a,vec2 b,vec2 c)
{
	return max(a, max(b,c));
}
vec2 min3(vec2 a,vec2 b,vec2 c)
{
	return min(a, min(b,c));
}
#endif

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} In[3];

out VoxelData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	flat vec4 bbox;
	flat int axis;
} Out;

// out VoxelData
// {
// 	vec3 position;
// 	vec3 normal;
// 	vec2 texcoord;
// 	vec3 colour;
// 	mat3 swizzle_inv;
// 	vec4 bbox;
// } Out;

uniform mat4 vp_x, vp_y, vp_z;
uniform float pixel_diagonal;
uniform int vol_tex_size;
//const float pixel_diagonal = 0.0;

void expandTriangle(inout vec4 screenpos[3])
{
	vec2 edge[3];
	edge[0] = screenpos[1].xy - screenpos[0].xy;
	edge[1] = screenpos[2].xy - screenpos[1].xy;
	edge[2] = screenpos[0].xy - screenpos[2].xy;

	vec2 edgenormal[3];
	edgenormal[0] = normalize(edge[0]);
	edgenormal[1] = normalize(edge[1]);
	edgenormal[2] = normalize(edge[2]);
	edgenormal[0] = vec2(-edgenormal[0].y, edgenormal[0].x);
	edgenormal[1] = vec2(-edgenormal[1].y, edgenormal[1].x);
	edgenormal[2] = vec2(-edgenormal[2].y, edgenormal[2].x);

	vec3 a = normalize(screenpos[1].xyz - screenpos[0].xyz);
	vec3 b = normalize(screenpos[2].xyz - screenpos[0].xyz);
	vec3 clipspacenormal = cross(a,b);
	if(clipspacenormal.z < 0.0)
	{
		edgenormal[0] *= -1.0;
		edgenormal[1] *= -1.0;
		edgenormal[2] *= -1.0;
	}

	screenpos[0].xy = screenpos[0].xy - pixel_diagonal * (edge[2]/ dot(edge[2], edgenormal[0]) + edge[0] / dot(edge[0], edgenormal[2]));
	screenpos[1].xy = screenpos[1].xy - pixel_diagonal * (edge[0]/ dot(edge[0], edgenormal[1]) + edge[1] / dot(edge[1], edgenormal[0]));
	screenpos[2].xy = screenpos[2].xy - pixel_diagonal * (edge[1]/ dot(edge[1], edgenormal[2]) + edge[2] / dot(edge[2], edgenormal[1]));
}

void main()
{
	vec3 face_normal = normalize(cross(In[1].position - In[0].position, In[2].position - In[0].position));
	float dominant_axis = max3(face_normal.x, face_normal.y, face_normal.z);
	mat4 projection;
	if(abs(dominant_axis - face_normal.x) < 0.001)
	{
		projection = vp_x;
		Out.axis = 1;
	}
	else if(abs(dominant_axis - face_normal.y) < 0.001)
	{
		projection = vp_y;
		Out.axis = 2;
	}
	else
	{
		projection = vp_z;
		Out.axis = 3;
	}
	vec4 screenpos[3];
	screenpos[0] = projection * vec4(In[0].position, 1.0);
	screenpos[1] = projection * vec4(In[1].position, 1.0);
	screenpos[2] = projection * vec4(In[2].position, 1.0);

	Out.bbox.xy = min3(screenpos[0].xy, screenpos[1].xy, screenpos[2].xy);
	Out.bbox.zw = max3(screenpos[0].xy, screenpos[1].xy, screenpos[2].xy);
	vec2 half_pixel = vec2(1.0/vol_tex_size);
	Out.bbox.xy -= vec2(half_pixel);
	Out.bbox.zw += vec2(half_pixel);

	expandTriangle(screenpos);

	Out.position = In[0].position;
	Out.normal = In[0].normal;
	Out.texcoord = In[0].texcoord;
	gl_Position = screenpos[0];
	EmitVertex();

	Out.position = In[1].position;
	Out.normal = In[1].normal;
	Out.texcoord = In[1].texcoord;
	gl_Position = screenpos[1];
	EmitVertex();

	Out.position = In[2].position;
	Out.normal = In[2].normal;
	Out.texcoord = In[2].texcoord;
	gl_Position = screenpos[2];
	EmitVertex();

	EndPrimitive();
	// screenpos[0] /= screenpos[0].w;
	// screenpos[1] /= screenpos[1].w;
	// screenpos[2] /= screenpos[2].w;
	// vec3 eyespace_v1 = normalize(gl_in[1].gl_Position.xyz -gl_in[0].gl_Position.xyz);
	// vec3 eyespace_v2 = normalize(gl_in[2].gl_Position.xyz -gl_in[0].gl_Position.xyz);
	// vec3 eyespace_normal = abs(cross(eyespace_v1, eyespace_v2));
	// float dominant_axis = max3(eyespace_normal.x, eyespace_normal.y, eyespace_normal.z);
	// mat3 swizzle_mat;
	// if(abs(dominant_axis - eyespace_normal.x) < 0.001)
	// {
	// 	swizzle_mat = mat3(	vec3(0.0,0.0,1.0),
	// 						vec3(0.0,1.0,0.0),
	// 						vec3(1.0,0.0,0.0));
	// }
	// else if(abs(dominant_axis - eyespace_normal.y) < 0.001)
	// {
	// 	swizzle_mat = mat3(	vec3(1.0,0.0,0.0),
	// 						vec3(0.0,0.0,1.0),
	// 						vec3(0.0,1.0,0.0));
	// }
	// else
	// {
	// 	swizzle_mat = mat3(	vec3(1.0,0.0,0.0),
	// 						vec3(0.0,1.0,0.0),
	// 						vec3(0.0,0.0,1.0));
	// }
	// Out.swizzle_inv = inverse(swizzle_mat);
	// Out.colour = swizzle_mat * vec3(0.0,0.0,1.0); // red = x, green = y, blue = z

	// vec4 screenpos[3];
	// screenpos[0] = projection * vec4(swizzle_mat * gl_in[0].gl_Position.xyz, 1.0);
	// screenpos[1] = projection * vec4(swizzle_mat * gl_in[1].gl_Position.xyz, 1.0);
	// screenpos[2] = projection * vec4(swizzle_mat * gl_in[2].gl_Position.xyz, 1.0);
	// screenpos[0] /= screenpos[0].w;
	// screenpos[1] /= screenpos[1].w;
	// screenpos[2] /= screenpos[2].w;

	// Out.bbox.xy = min(screenpos[0].xy, min(screenpos[1].xy, screenpos[2].xy));
	// Out.bbox.zw = max(screenpos[0].xy, max(screenpos[1].xy, screenpos[2].xy));
	// Out.bbox.xy -= vec2(pixel_diagonal);
	// Out.bbox.zw += vec2(pixel_diagonal);

	// expandTriangle(screenpos);

	// Out.position = In[0].position;
	// Out.normal = In[0].normal;
	// Out.texcoord = In[0].texcoord;
	// gl_Position = screenpos[0];
	// EmitVertex();

	// Out.position = In[1].position;
	// Out.normal = In[1].normal;
	// Out.texcoord = In[1].texcoord;
	// gl_Position = screenpos[1];
	// EmitVertex();

	// Out.position = In[2].position;
	// Out.normal = In[2].normal;
	// Out.texcoord = In[2].texcoord;
	// gl_Position = screenpos[2];
	// EmitVertex();

	// EndPrimitive();
}
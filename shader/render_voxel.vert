layout(location = 0) in vec3 Position;

out VertexData
{
	vec4 position;
	uvec3 texcoord;
} Out;

uniform uint voxel_dim;

void main(void)
{
	// Out.texcoord.x = gl_VertexID % voxel_dim;
	// Out.texcoord.y = (gl_VertexID / voxel_dim) % voxel_dim;
	// Out.texcoord.z = gl_VertexID / (voxel_dim * voxel_dim);
	Out.texcoord.x = gl_InstanceID % voxel_dim;
	Out.texcoord.y = (gl_InstanceID / voxel_dim) % voxel_dim;
	Out.texcoord.z = gl_InstanceID / (voxel_dim * voxel_dim);

	Out.position = vec4(Out.texcoord / float(voxel_dim) * 2.0 - 1.0, 1.0);
	Out.position.x -= 1.0 / float(voxel_dim);
	Out.position.z += 1.0 / float(voxel_dim);

	gl_Position = vec4(Position/ (float(voxel_dim)/2.0), 0.0) + Out.position;
}
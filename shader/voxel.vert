layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} Out;

// uniform mat4 model;
// uniform mat4 view;

void main(void)
{
	// Out.position = (model * vec4(position, 1.0)).xyz;

	// Out.normal = mat3(model) * normal;
	Out.position = position;
	Out.normal = normal;
	Out.texcoord = texcoord;
	gl_Position = vec4(position, 1.0);
}
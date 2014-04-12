layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out VertexData
{
	vec3 position;
	vec3 normal;
} Out;

uniform mat4 model;
uniform mat4 view;

void main(void)
{
	Out.position = (model * vec4(position, 1.0)).xyz;
	Out.normal = mat3(model) * normal;
	gl_Position = view * model * vec4(position, 1.0);
}
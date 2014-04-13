layout(location = 0) in vec3 position;

out vec2 coordinates;

void main()
{
	coordinates = position.xz;
	gl_Position =vec4(position.x, position.z, 0.0, 1.0);
}
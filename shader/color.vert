in vec3 position;
in vec3 color;

out vec3 vcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	vcolor = color;
}
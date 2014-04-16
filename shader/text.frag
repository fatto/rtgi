out vec4 color;

in vec2 g_texcoord;

uniform sampler2D sampler;
uniform vec3 textcolor;

void main()
{
	float A = texture(sampler, g_texcoord).r;
	color = vec4(textcolor, A);
}
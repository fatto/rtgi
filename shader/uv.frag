flat in vec2 vuv;

out vec4 color;

uniform sampler2D sampl;

void main()
{
	color = texture(sampl, vuv);
}
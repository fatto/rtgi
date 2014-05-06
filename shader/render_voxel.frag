in FragmentData
{
	vec4 colour; // alpha used as counter
} In;

layout(location = 0) out vec4 colour;

void main()
{
	colour = vec4(In.colour.rgb/In.colour.a, 1.0);
}
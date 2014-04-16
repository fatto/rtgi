layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in int v_character[1];
in int v_position[1];

out vec2 g_texcoord;

uniform vec2 cellsize;
uniform vec2 celloffset;
uniform vec2 rendersize;
uniform vec2 renderorigin;

void main()
{
	float x = renderorigin.x + float(v_position[0]) * rendersize.x * 2;
	float y = renderorigin.y;
	vec4 P = vec4(x, y, 0, 1);
	vec4 U = vec4(1, 0, 0, 0) * rendersize.x;
	vec4 V = vec4(0, 1, 0, 0) * rendersize.y;

	int letter = v_character[0];
	letter = clamp(letter - 32, 0, 96);
	int row = letter / 16 + 1;
	int col = letter % 16;
	float S0 = celloffset.x + cellsize.x * col;
	float T0 = celloffset.y + 1 - cellsize.y*row;
	float S1 = S0 + cellsize.x - celloffset.x;
	float T1 = T0 + cellsize.y;

	g_texcoord = vec2(S0, T1); gl_Position = P - U - V; EmitVertex();
	g_texcoord = vec2(S1, T1); gl_Position = P + U - V; EmitVertex();
	g_texcoord = vec2(S0, T0); gl_Position = P - U + V; EmitVertex();
	g_texcoord = vec2(S1, T0); gl_Position = P + U + V; EmitVertex();
	EndPrimitive();
}
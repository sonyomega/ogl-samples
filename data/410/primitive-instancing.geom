#version 410 core

// Declare all the semantics
#define ATTR_POSITION	0
#define ATTR_COLOR		3
#define ATTR_TEXCOORD	4

#define VERT_POSITION	0
#define VERT_COLOR		3
#define VERT_TEXCOORD	4
#define VERT_INSTANCE	7

#define GEOM_COLOR		3

#define FRAG_COLOR		0
#define FRAG_RED		0
#define FRAG_GREEN		1
#define FRAG_BLUE		2
#define FRAG_ALPHA		3

layout(triangles, invocations = 6) in;

layout(location = VERT_COLOR) in vec3 Color[];
layout(location = GEOM_COLOR, stream = 0) out vec3 GeomColor;

uniform mat4 MVP;

void main()
{	
	for(int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = MVP * (gl_in[i].gl_Position + vec4(vec2(0.0), - 0.3 + float(0.1) * float(gl_InvocationID), 0.0));
		GeomColor = (vec3(gl_InvocationID + 1) / 6.0 + Color[i]) / 2.0; 
		EmitVertex();
	}
	EndPrimitive();
}

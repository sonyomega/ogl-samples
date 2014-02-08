#version 400 core

uniform sampler2D Diffuse;

in block
{
	vec2 Texcoord;
} In;

out vec4 Color;

float textureLevel(in sampler2D Sampler, in vec2 Texcoord)
{
	vec2 TextureSize = vec2(textureSize(Sampler, 0));

	float LevelCount = max(log2(TextureSize.x), log2(TextureSize.y));

	vec2 dx = dFdx(Texcoord * TextureSize);
	vec2 dy = dFdy(Texcoord * TextureSize);
	float d = max(dot(dx, dx), dot(dy, dy));

	d = clamp(d, 1.0, pow(2, (LevelCount - 1) * 2));

	return 0.5 * log2(d);
}

void main()
{
	//vec2 Level = textureQueryLod(Diffuse, In.Texcoord);
	//float Level = textureLevel(Diffuse, In.Texcoord);

	//Color = texture(Diffuse, In.Texcoord);
	//Color = vec4(vec3(Level) * 0.125, 1.0);//texture(Diffuse, In.Texcoord);
	Color = vec4(fwidth(In.Texcoord), 0.0, 1.0);
}
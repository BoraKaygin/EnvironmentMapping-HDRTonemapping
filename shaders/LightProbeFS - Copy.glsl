#version 460 core

in vec4 color;
out vec4 fragColor;

void main(void)
{
	// Set the color of this fragment to the interpolated color
	// value computed by the rasterizer.
	fragColor = color;
	float delta = 1e-5;
	fragColor.r = log(fragColor.r + delta);
	fragColor.g = log(fragColor.g + delta);
	fragColor.b = log(fragColor.b + delta);

	fragColor.a = 1.0;
}

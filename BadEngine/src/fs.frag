#version 450 core

in vec3 vertColor;

out vec4 Color;

void main()
{
	//if(alpha < 0.1f)
	//¿aden fragment nie jest wygenerowany dla tego piksela
	//¿aden kolor nie trafia w wyniku wywo³ania do bufora kolorów i nie ma tesów g³êbokoœci
	//discard;

// a sk³adowa alfa - przezroczystoœæ, wspó³czynnik przezroczystkoœci
	//Color = vec4(vertColor, aplha);
	Color = vec4(vertColor, 0.5f);
}
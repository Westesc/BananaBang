#version 450 core

in vec3 vertColor;

out vec4 Color;

void main()
{
	//if(alpha < 0.1f)
	//�aden fragment nie jest wygenerowany dla tego piksela
	//�aden kolor nie trafia w wyniku wywo�ania do bufora kolor�w i nie ma tes�w g��boko�ci
	//discard;

// a sk�adowa alfa - przezroczysto��, wsp�czynnik przezroczystko�ci
	//Color = vec4(vertColor, aplha);
	Color = vec4(vertColor, 0.5f);
}
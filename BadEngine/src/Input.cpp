#include "../lib/Input.h"

Input::Input()
{
}

Input::~Input()
{
}

Axis Input::getAxis(std::string name)
{
	return Axis(name);
}

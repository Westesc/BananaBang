#ifndef Input_H
#define Input_H

#include "Component.h"
#include <string>
#include "Axis.h"

class Input : Component {
public:
	Input();
	~Input();

	Axis getAxis(std::string name);
};

#endif

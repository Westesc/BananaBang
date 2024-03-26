#ifndef Axis_H
#define Axis_H

#include <string>

class Axis {
public:
	Axis(std::string Name);
	~Axis();

	char positive;
	char negative;
	std::string name;
};

#endif

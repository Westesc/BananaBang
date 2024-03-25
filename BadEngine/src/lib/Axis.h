#include <string>
#ifndef Axis_H
#define Axis_H

class Axis {
public:
	Axis(std::string Name);
	~Axis();

	char positive;
	char negtaive;
	std::string name;
};

#endif

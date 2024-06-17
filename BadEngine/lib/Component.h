#ifndef COMPONENT_H
#define COMPONENT_H

#include "Transform.h"

class Component {
public:
	bool enable = true;
	virtual void upadate(Transform * transform){}
};

#endif

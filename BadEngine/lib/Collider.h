#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component.h"

class Collider : Component {
public:
	template <typename T>
	T* getAsActualType() {
		return dynamic_cast<T*>(this);
	}
	enum ColliderType {
		BOX,
		CAPSULE,
		MESH
	};
	ColliderType type;
};
#endif

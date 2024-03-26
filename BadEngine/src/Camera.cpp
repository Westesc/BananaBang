#include "../lib/Camera.h"
Camera::Camera() {
	transform = new Transform();
}
Camera::~Camera() {
	delete transform;
}
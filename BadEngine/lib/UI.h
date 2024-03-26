#ifndef UI_H
#define UI_H

#include <string>
#include "Component.h"

class UI : Component {
public:
	UI();
	~UI();

	std::string text;

	std::string getText();
};

#endif

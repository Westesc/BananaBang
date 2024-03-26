#ifndef Scene_H
#define Scene_H

#include <string>

class Scene {
public:
	Scene(std::string Name);
	~Scene();

	std::string name;
protected:
	void Start();
	void Update();
};

#endif

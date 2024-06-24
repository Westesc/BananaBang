#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

struct ResourceLoader {
public:
	ResourceLoader() {}
	~ResourceLoader() {}

	std::shared_ptr<Model> animodel;
	Shader* shaderAnimation;
	Shader* fillingShader;
	Shader* outlineShader;
	Shader* shaders;
	Shader* skydomeShader;
	Shader* mapsShader;
	Shader* shaderTree;
	Shader* enemyShader;
	Shader* diffuseShader;
	Shader* groundShader;
	std::shared_ptr<Model> enemyModel;
	std::shared_ptr<Model> enemyModel2;
	std::shared_ptr<Model> animationEnemyModel;
	std::shared_ptr<Model> outlinemodel;
	std::shared_ptr<Model> bananaModel;
	std::shared_ptr<Model> bananaModel2;
	Shader* depthShader;
	Shader* depthAnimationShader;
	std::shared_ptr<Model> box2model;
	std::shared_ptr<Model> treelog;
	std::shared_ptr<Model> treetrunk;
	Shader* phongShader;
	Shader* phongInstancedShader;
	std::shared_ptr<Model> treebranch1;
	std::shared_ptr<Model> planeSectormodel;
	std::shared_ptr<Model> enemyWeaponmodel;
	std::shared_ptr<Model> treetrunklow;
	std::shared_ptr<Model> treeloglow;
	std::shared_ptr<Model> mangoModel;
	std::shared_ptr<Model> leafModel;
	std::shared_ptr<Model> planeModel;
	Shader* diffuseInstancedShader;
	Shader* depthInstancedShader;

	void load() {
		animodel = std::make_shared<Model>(const_cast<char*>("res/animations/Monkey/monkey_idle.dae"), true);
		shaderAnimation = new Shader("res/shaders/vs_animation.vert", "res/shaders/fs_animation.frag");
		fillingShader = new Shader("res/shaders/vs_filling.vert", "res/shaders/fs_filling.frag");
		outlineShader = new Shader("res/shaders/outline_animation.vert", "res/shaders/outline_animation.frag");
		shaders = new Shader("res/shaders/vs.vert", "res/shaders/fs.frag");
		skydomeShader = new Shader("res/shaders/vsS.vert", "res/shaders/fsS.frag");
		mapsShader = new Shader("res/shaders/v_maps.vert", "res/shaders/f_maps.frag");
		shaderTree = new Shader("res/shaders/vsTree.vert", "res/shaders/fsTree.frag");
		enemyShader = new Shader("res/shaders/enemy.vert", "res/shaders/enemy.frag");
		diffuseShader = new Shader("res/shaders/diffuse.vert", "res/shaders/diffuse.frag");
		groundShader = new Shader("res/shaders/ground.vert", "res/shaders/ground.frag");
		animationEnemyModel = std::make_shared<Model>(const_cast<char*>("res/animations/Lumberjack/Lumberjack_Idle.dae"), true);
		enemyModel = std::make_shared<Model>(const_cast<char*>("res/Lumberjack.obj"), false);
		enemyModel2 = std::make_shared<Model>(const_cast<char*>("res/capsule.obj"), false);
		outlinemodel = std::make_shared<Model>(const_cast<char*>("res/Lumberjack.obj"), false);
		bananaModel = std::make_shared<Model>(const_cast<char*>("res/banana.obj"), false);
		bananaModel2 = std::make_shared<Model>(const_cast<char*>("res/banana.obj"), false);
		depthShader = new Shader("res/shaders/depthShader.vert", "res/shaders/depthShader.frag");
		depthAnimationShader = new Shader("res/shaders/depthAnimationShader.vert", "res/shaders/depthShader.frag");
		box2model = std::make_shared<Model>(const_cast<char*>("res/tree.obj"), false);
		treelog = std::make_shared<Model>(const_cast<char*>("res/objects/trees/tree_log_l.obj"), false);
		treetrunk = std::make_shared<Model>(const_cast<char*>("res/objects/trees/tree_trunk.obj"), false);
		phongShader = new Shader("res/shaders/phong.vert", "res/shaders/phong.frag");
		phongInstancedShader = new Shader("res/shaders/phonginstanced.vert", "res/shaders/phong.frag");
		treebranch1 = std::make_shared<Model>(const_cast<char*>("res/objects/trees/tree_branch_l.obj"), false);
		planeSectormodel = std::make_shared<Model>(const_cast<char*>("res/plane.obj"), false);
		enemyWeaponmodel = std::make_shared<Model>(const_cast<char*>("res/axe.obj"), false);
		treetrunklow = std::make_shared<Model>(const_cast<char*>("res/objects/trees/tree_trunk_l.obj"), false);
		treeloglow = std::make_shared<Model>(const_cast<char*>("res/objects/trees/tree_log_l.obj"), false);
		mangoModel  = std::make_shared<Model>(const_cast<char*>("res/objects/mango.obj"), false);
		leafModel = std::make_shared<Model>(const_cast<char*>("res/objects/leafes.obj"), false);
		planeModel = std::make_shared<Model>(const_cast<char*>("res/plane.obj"), false);
		diffuseInstancedShader = new Shader("res/shaders/diffuseinstanced.vert", "res/shaders/diffuse.frag");
		depthInstancedShader = new Shader("res/shaders/depthInstanceShader.vert", "res/shaders/depthShader.frag");
	}
};
#endif

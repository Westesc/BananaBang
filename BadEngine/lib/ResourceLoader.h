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
	std::shared_ptr<Model> enemyModel;
	std::shared_ptr<Model> outlinemodel;
	std::shared_ptr<Model> bananaModel;
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

	void load() {
		animodel = std::make_shared<Model>(const_cast<char*>("../../../../res/animations/Walking.dae"), true);
		shaderAnimation = new Shader("../../../../src/shaders/vs_animation.vert", "../../../../src/shaders/fs_animation.frag");
		fillingShader = new Shader("../../../../src/shaders/vs_filling.vert", "../../../../src/shaders/fs_filling.frag");
		outlineShader = new Shader("../../../../src/shaders/vs_outline.vert", "../../../../src/shaders/fs_outline.frag");
		shaders = new Shader("../../../../src/shaders/vs.vert", "../../../../src/shaders/fs.frag");
		skydomeShader = new Shader("../../../../src/shaders/vsS.vert", "../../../../src/shaders/fsS.frag");
		mapsShader = new Shader("../../../../src/shaders/v_maps.vert", "../../../../src/shaders/f_maps.frag");
		shaderTree = new Shader("../../../../src/shaders/vsTree.vert", "../../../../src/shaders/fsTree.frag");
		enemyShader = new Shader("../../../../src/shaders/enemy.vert", "../../../../src/shaders/enemy.frag");
		diffuseShader = new Shader("../../../../src/shaders/diffuse.vert", "../../../../src/shaders/diffuse.frag");
		enemyModel = std::make_shared<Model>(const_cast<char*>("../../../../res/capsule.obj"), false);
		outlinemodel = std::make_shared<Model>(const_cast<char*>("../../../../res/Lumberjack.obj"), false);
		bananaModel = std::make_shared<Model>(const_cast<char*>("../../../../res/banana.obj"), false);
		depthShader = new Shader("../../../../src/shaders/depthShader.vert", "../../../../src/shaders/depthShader.frag");
		depthAnimationShader = new Shader("../../../../src/shaders/depthAnimationShader.vert", "../../../../src/shaders/depthAnimationShader.frag");
		box2model = std::make_shared<Model>(const_cast<char*>("../../../../res/tree.obj"), false);
		treelog = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_log.obj"), false);
		treetrunk = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_trunk.obj"), false);
		phongShader = new Shader("../../../../src/shaders/phong.vert", "../../../../src/shaders/phong.frag");
		phongInstancedShader = new Shader("../../../../src/shaders/phonginstanced.vert", "../../../../src/shaders/phong.frag");
		treebranch1 = std::make_shared<Model>(const_cast<char*>("../../../../res/objects/trees/tree_branch_1.obj"), false);
		planeSectormodel = std::make_shared<Model>(const_cast<char*>("../../../../res/plane.obj"), false);
		enemyWeaponmodel = std::make_shared<Model>(const_cast<char*>("../../../../res/axe.obj"), false);
	}
};
#endif

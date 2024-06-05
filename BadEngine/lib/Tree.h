#ifndef Tree_H
#define Tree_H

#include "Enemy.h"

class Tree : public GameObject {
public:
	Tree(std::string name, float initialHealth) : GameObject(name) {
        health = initialHealth;
        maxHealth = initialHealth;
        initialY = getTransform()->localPosition.y;
	}
	~Tree() {}

	float health;
    float maxHealth;
    float initialY;
	std::vector<Enemy*> choppers;

    void updateHealth(float deltaTime) {
        float damagePerSecond = choppers.size() * 4;
        float damage = damagePerSecond * deltaTime;
        health -= damage;

        if (health <= 0) {
            chopDown();
        }
        else {
            adjustPosToHealth();
        }
    }

    void chopDown() {
        for (auto enemy : choppers) {
            enemy->chosenTree = nullptr;
        }
        choppers.clear();
    }

    void addChopper(Enemy* enemy) {
        choppers.push_back(enemy);
    }

    void removeChopper(Enemy* enemy) {
        choppers.erase(std::remove(choppers.begin(), choppers.end(), enemy), choppers.end());
    }

    void adjustPosToHealth() {
        float healthRatio = health / maxHealth;
        getTransform()->localPosition.y = healthRatio * initialY;
        float Ydiff = initialY - getTransform()->localPosition.y;
        children[0]->getTransform()->localPosition.y -= Ydiff;
        for (auto branch : children[0]->children) {
            branch->getTransform()->localPosition.y -= Ydiff;
        }
    }
};
#endif

#ifndef ENEMYSTATEMANAGER_H
#define ENEMYSTATEMANAGER_H

#include "Pathfinder.h"
#include "CollisionManager.h"

class EnemyStateManager {
public:
    EnemyStateManager(Pathfinder* pathfinder, CollisionManager* cm, PlayerMovement* pm) {
        this->pathfinder = pathfinder;
        this->cm = cm;
        this->pm = pm;
    }

    std::vector<Enemy*> enemies;
    Pathfinder* pathfinder;
    CollisionManager* cm;
    GameObject* player = nullptr;
    PlayerMovement* pm = nullptr;

    void update(float deltaTime, bool playerAtention) {
        Enemy* closestEnemy = nullptr;
        glm::vec2 closestDistance = glm::vec3(1.f);
        if (player && enemies.size() != 0) {
            closestDistance = abs(glm::vec2(player->getTransform()->getLocalPosition().x, player->getTransform()->getLocalPosition().z) - glm::vec2(enemies.at(0)->getTransform()->getLocalPosition().x, enemies.at(0)->getTransform()->getLocalPosition().z));
            closestEnemy = enemies.at(0);
        }
        for (auto& enemy : enemies) {
            if (pm->getState() == PlayerState::climbing && player) {
                if (glm::length(closestDistance) > glm::length(abs(glm::vec2(player->getTransform()->getLocalPosition().x, player->getTransform()->getLocalPosition().z) - glm::vec2(enemy->getTransform()->getLocalPosition().x, enemy->getTransform()->getLocalPosition().z)))) {
                    closestEnemy = enemy;
                    closestDistance = abs(glm::vec2(player->getTransform()->getLocalPosition().x, player->getTransform()->getLocalPosition().z) - glm::vec2(enemy->getTransform()->getLocalPosition().x, enemy->getTransform()->getLocalPosition().z));
                }
            }
            if (playerAtention && glm::distance(enemy->getTransform()->localPosition, player->getTransform()->localPosition) < 15.0f) {
                enemy->state = EnemyState::Attacking;
            }
            else if (enemy->state == EnemyState::Attacking && glm::distance(enemy->getTransform()->localPosition, player->getTransform()->localPosition) > 20.0f) {
                enemy->state = EnemyState::Idle;
            }
            switch (enemy->state) {
            case EnemyState::Idle:
                updateIdleState(enemy, deltaTime);
                break;
            case EnemyState::Walking:
                updateWalkingState(enemy, deltaTime);
                break;
            case EnemyState::Chopping:
                updateChoppingState(enemy, deltaTime);
                break;
            case EnemyState::Attacking:
                updateAttackingState(enemy, deltaTime);
                break;
            }
        }
        pm->setAttackDestination(closestEnemy);
    }

    void addEnemy(Enemy* enemy) {
        enemies.push_back(enemy);
    }

private:
    void updateIdleState(Enemy* enemy, float deltaTime) {
        enemy->velocity = glm::vec3(0.0f);
        std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
        if (tree.second != nullptr) {
            enemy->chosenTreePos = tree.first;
            enemy->chosenTree = tree.second;
            enemy->state = EnemyState::Walking;
        }
    }
    void updateWalkingState(Enemy* enemy, float deltaTime) {
        if (glm::distance(enemy->getTransform()->localPosition, enemy->chosenTreePos) > 5.f) {
            enemy->timeSinceDirChange += deltaTime;
            glm::vec3 destination = pathfinder->decideDestination(enemy->chosenTreePos, enemy->getTransform()->localPosition, enemy->sector);
            glm::vec3 direction = glm::normalize(destination - enemy->getTransform()->localPosition);
            enemy->setVel(direction);
            glm::vec3 tmpMove = glm::vec3(enemy->velocity.x, 0.0f, enemy->velocity.z);
            enemy->Move(tmpMove * deltaTime);
            cm->addObjectPredict(enemy);
            std::vector<GameObject*> collisions = cm->checkPrediction();
            enemy->Move(-tmpMove * deltaTime);
            enemy->setVel2(collisions);
            if ((glm::any(glm::isnan(enemy->getTransform()->localPosition)) || glm::any(glm::isinf(enemy->getTransform()->localPosition)))) {
                enemy->getTransform()->localPosition = glm::vec3(5.0f);
            }
            cm->addObject(enemy);
            enemy->timeSpentWalking += deltaTime;
            if (enemy->timeSpentWalking > 15.f) {
                std::pair<glm::vec3, Tree*> tree = pathfinder->decideInitalDestination(enemy->sector);
                if (tree.second == nullptr) {
                    enemy->state = EnemyState::Idle;
                }
                else {
                    enemy->chosenTreePos = tree.first;
                    enemy->chosenTree = tree.second;
                    enemy->timeSpentWalking = 0.f;
                }
            }
        }
        else {
            enemy->state = EnemyState::Chopping;
        }
    }
    void updateChoppingState(Enemy* enemy, float deltaTime) {
        if (glm::distance(enemy->getTransform()->localPosition, enemy->chosenTreePos) > 5.f) {
            if (enemy->chosenTree) {
                enemy->chosenTree->getAsActualType<Tree>()->removeChopper(enemy);
                enemy->state = EnemyState::Walking;
            }
            else {
                enemy->state = EnemyState::Idle;
            }
        }
        else {
            if (enemy->chosenTree) {
                if (std::find(enemy->chosenTree->getAsActualType<Tree>()->choppers.begin(), enemy->chosenTree->getAsActualType<Tree>()->choppers.end(), enemy) == enemy->chosenTree->getAsActualType<Tree>()->choppers.end()) {
                    enemy->chosenTree->getAsActualType<Tree>()->addChopper(enemy);
                }
            }
            else {
				enemy->state = EnemyState::Idle;
            }
        }
    }
    void updateAttackingState(Enemy* enemy, float deltaTime) {
        if (player) {
            if (glm::distance(enemy->getTransform()->localPosition, player->getTransform()->localPosition) > 3.0f) {
                enemy->children.at(0)->active = false;
                enemy->children.at(0)->getTransform()->localRotation = glm::vec3(0.0f);
                glm::vec3 direction = glm::normalize(player->getTransform()->localPosition - enemy->getTransform()->localPosition);
                enemy->velocity = direction * (enemy->velLimits.second - enemy->velLimits.first);
                enemy->velocity.y = 0;
                glm::vec3 tmpMove = glm::vec3(enemy->velocity.x, 0.0f, enemy->velocity.z);
                enemy->Move(tmpMove * deltaTime);
                cm->addObjectPredict(enemy);
                std::vector<GameObject*> collisions = cm->checkPrediction();
                enemy->Move(-tmpMove * deltaTime);
                enemy->setVel2(collisions);
                if ((glm::any(glm::isnan(enemy->getTransform()->localPosition)) || glm::any(glm::isinf(enemy->getTransform()->localPosition)))) {
                    enemy->getTransform()->localPosition = glm::vec3(5.0f);
                }
                cm->addObject(enemy);
            }
            else {
                enemy->velocity = glm::vec3(0.0f);
                GameObject* weapon = enemy->children.at(0);
                weapon->active = true;
                weapon->isVisible = true;
                weapon->getTransform()->localPosition = enemy->getTransform()->localPosition + glm::vec3(0.0f, 0.0f, 0.0f) + glm::normalize(player->getTransform()->localPosition - enemy->getTransform()->localPosition);
                enemy->attackTime += deltaTime;
                if (enemy->attackTime > 2.0f) {
                    weapon->active = false;
                    weapon->isVisible = false;
                    enemy->attackTime = 0.0f;
                    weapon->getTransform()->localRotation = glm::vec3(0.0f);
                }
                if (enemy->attackTime > 1.0f) {
                    glm::vec3 directionToPlayer = glm::normalize(player->getTransform()->localPosition - enemy->getTransform()->localPosition);
                    float angleToPlayer = glm::atan(directionToPlayer.z, directionToPlayer.x);
                    weapon->getTransform()->localRotation.x = 90.0f;
                    weapon->getTransform()->localRotation.y = angleToPlayer;
                    if (cm->checkCollision(weapon, player)) {
                        weapon->active = false;
                        weapon->isVisible = false;
                        enemy->attackTime = 0.0f;
                        weapon->getTransform()->localRotation = glm::vec3(0.0f);
                        if (player->hp > 0) {
                            player->hp -= 1;
                        }
                    }
				}
            }
        }
    }
};

#endif
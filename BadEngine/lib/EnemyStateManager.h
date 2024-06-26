#ifndef ENEMYSTATEMANAGER_H
#define ENEMYSTATEMANAGER_H

#include "Pathfinder.h"
#include "CollisionManager.h"
#include "sectorSelector.h"

class EnemyStateManager {
public:
    EnemyStateManager(Pathfinder* pathfinder, CollisionManager* cm, PlayerMovement* pm, SectorSelector* ss, float* timeSinceHit) {
        this->pathfinder = pathfinder;
        this->cm = cm;
        this->pm = pm;
        this->ss = ss;
        this->timeSinceHit = timeSinceHit;
    }

    std::vector<Enemy*> enemies;
    Pathfinder* pathfinder;
    CollisionManager* cm;
    GameObject* player = nullptr;
    PlayerMovement* pm = nullptr;
    float multipierDistance = 4.5f;
    SectorSelector* ss;
    float* timeSinceHit;

    void update(float deltaTime, bool playerAtention) {
        if (pm->getState() == PlayerState::climbing) {
            Enemy* closestEnemy = nullptr;
            float yDistance;
            float closestDistance = std::numeric_limits<float>::max();
            if (player && !enemies.empty()) {
                glm::vec2 playerPos = glm::vec2(player->getTransform()->getLocalPosition().x, player->getTransform()->getLocalPosition().z);
                for (auto& enemy : enemies) {
                    glm::vec2 enemyPos = glm::vec2(enemy->getTransform()->getLocalPosition().x, enemy->getTransform()->getLocalPosition().z);
                    float distance = glm::distance(playerPos, enemyPos);
                    if (distance < closestDistance) {
                        closestEnemy = enemy;
                        closestDistance = distance;
                        yDistance = abs(player->getTransform()->getLocalPosition().y - enemy->getTransform()->getLocalPosition().y);
                    }
                }
                if (multipierDistance * yDistance < closestDistance) {
                    closestEnemy = nullptr;
                }
            }
            pm->setAttackDestination(closestEnemy);
        }
        for (auto& enemy : enemies) {
            if (enemy->hp > 0) {
                if (playerAtention && glm::distance(enemy->getTransform()->localPosition, player->getTransform()->localPosition) < 15.0f) {
                    enemy->state = EnemyState::Attacking;
                }
                else if (enemy->state == EnemyState::Attacking && glm::distance(enemy->getTransform()->localPosition, player->getTransform()->localPosition) > 20.0f) {
                    enemy->state = EnemyState::Idle;
                }
                switch (enemy->state) {
                case EnemyState::Idle:
                    updateIdleState(enemy, deltaTime);
                    enemy->getAnimateBody()->setActiveAnimation("idle");
                    break;
                case EnemyState::Walking:
                    for (auto audio : enemy->audios) {
                        if (audio->name == "running") {
                            audio->playConst();
                        }
                        else {
                            audio->stop();
                        }
                    }
                    updateWalkingState(enemy, deltaTime);
                    enemy->getAnimateBody()->setActiveAnimation("walking");
                   
                    break;
                case EnemyState::Chopping:
                    for (auto audio : enemy->audios) {
                        if (audio->name == "axe_attack") {
                            audio->play();
                        }
                        else {
                            audio->stop();
                        }
                    }
                    updateChoppingState(enemy, deltaTime);

                    break;
                case EnemyState::Attacking:
                    for (auto audio : enemy->audios) {
                        if (audio->name == "axe_attack") {
                            audio->play();
                        }
                        else {
                            audio->stop();
                        }
                    }
                    updateAttackingState(enemy, deltaTime);
                    rotateEnemyTowards(enemy, player->getTransform()->localPosition);
                   
                    break;
                }
            }
        }
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
        else {
            enemy->sector = ss->selectedSector;
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
            if (enemy->velocity != glm::vec3(0.0f)) {
				float angle = glm::atan(enemy->velocity.z, enemy->velocity.x);
				enemy->getTransform()->localRotation.y = angle;
			}
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
            rotateEnemyTowards(enemy, enemy->chosenTreePos);
        }
        else {
            enemy->state = EnemyState::Chopping;
        }
    }
    void rotateEnemyTowards(Enemy* enemy, glm::vec3 targetPosition) {
        glm::vec3 enemyPosition = enemy->getTransform()->getLocalPosition();
        glm::vec3 direction = glm::normalize(targetPosition - enemyPosition);
        float angle = atan2(direction.x, direction.z);
        enemy->getTransform()->localRotation.y = glm::degrees(angle);
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
                enemy->getAnimateBody()->setActiveAnimation("walking");
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
                if (enemy->velocity != glm::vec3(0.0f)) {
                    float angle = glm::atan(enemy->velocity.z, enemy->velocity.x);
                    enemy->getTransform()->localRotation.y = angle;
                }
            }
            else {
                enemy->getAnimateBody()->setActiveAnimation("hit");
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
                        *timeSinceHit = 0.0f;
                    }
				}
            }
        }
    }
};

#endif
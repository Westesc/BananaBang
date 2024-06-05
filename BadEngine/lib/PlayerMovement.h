#ifndef PlayerMovement_H
#define PlayerMovement_H
#include <cmath>
#include "Transform.h"
#include "TimeManager.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class PlayerState {
    walking,
    jump_up,
    air,
    attack1,
    dodge,
    climbing
};

class PlayerMovement {
private:
    //references
    SceneManager* sm;
    Input* input;
    Camera* camera;
    TimeManager* tm;
    RigidBody* rb;

    //animation
    std::queue < std:: string > queueAnim;
    int animCount = 0;

    //walking
    float moveSpeed = 4.f;
    float deltaTime2 = 0.f;
    float runSpeed = 10.f;
    float turnSpeed = 90.f;

    float currentTurn = 0.f;
    float rotateAngle = 0.f;
    
    //jumping
    float limitJump = 7.f;
    float jumpPower = 5.f;
    glm::vec3 initialPosition;
    float airSpeed = 4.f;

    //climbing
    float climbingSpeed = 3.f;
    float currentClimbingSpeed = 3.f;

    
    PlayerState state = PlayerState::walking;

    bool wasSpacePressed = false;

    void getRotate(bool isRotate) {
        if (isRotate) {
            float rotationSpeed = 500.f;
            float angleDifference = currentTurn - rotateAngle;
            float rotationAmount = rotationSpeed * tm->getFramePerSeconds();

            while (angleDifference < -180.0f) angleDifference += 360.0f;
            while (angleDifference > 180.0f) angleDifference -= 360.0f;

            if (fabs(angleDifference) <= rotationAmount) {
                rotateAngle = currentTurn;
            }
            else {
                int direction = (angleDifference > 0) ? 1 : -1;
                rotateAngle += direction * rotationAmount;
            }
            float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
            sm->getActiveScene()->findByName("player")->getTransform()->localRotation.y = angle + rotateAngle;
        }
    }
    void move(float deltaTime, bool isRotate = true) {
        getRotate(isRotate);

        float rotate = sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y;
        float sinRotate = sin(glm::radians(rotate));
        float cosRotate = cos(glm::radians(rotate));

        float dx = sm->getActiveScene()->findByName("player")->getTransform()->getLocalScale().x * sm->getActiveScene()->findByName("player")->getAnimateBody()->getPosition().z * sinRotate + sm->getActiveScene()->findByName("player")->getAnimateBody()->getPosition().x * cosRotate;
        float dz = sm->getActiveScene()->findByName("player")->getTransform()->getLocalScale().z * sm->getActiveScene()->findByName("player")->getAnimateBody()->getPosition().z * cosRotate - sm->getActiveScene()->findByName("player")->getAnimateBody()->getPosition().x * sinRotate;

       // sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.f, dy, 0.f));
        glm::vec3 vel = glm::vec3(dx, 0.f, dz)/ deltaTime;
        sm->getActiveScene()->findByName("player")->velocity = vel * 4.0f;
    }

    void moveInAir(float speed, float deltaTime) {
        getRotate(true);
        float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
         sm->getActiveScene()->findByName("player")->getTransform()->localRotation.y = angle + rotateAngle;
        float distance = speed * tm->getFramePerSeconds();
        float dx = distance * sin(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        float dz = distance * cos(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        sm->getActiveScene()->findByName("player")->velocity.x = dx / deltaTime;
        sm->getActiveScene()->findByName("player")->velocity.z = dz / deltaTime;
    }

    void MovePlayer(float deltaTime) {
        float speed = 0.f;
        if (input->checkAnyKey() && deltaTime2 > 0.02f) {
            deltaTime2 = 0.f;
            if (input->checkKey(GLFW_KEY_LEFT_SHIFT) && state == PlayerState::walking) {
                // speed = runSpeed;
            }
            if (input->checkKey(GLFW_KEY_W)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 180.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -135.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 135.f;
            }
            else if (input->checkKey(GLFW_KEY_S)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 0.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -45.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 45.f;
            }
            else if (input->checkKey(GLFW_KEY_D)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 90;
            }
            else if (input->checkKey(GLFW_KEY_A)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = -90;
            }
        }
        if (input->checkAnyKey()) {
            if (state == PlayerState::air || state == PlayerState::jump_up) {
                moveInAir(speed, deltaTime);
            }
            else
            {
                move(deltaTime);
            }
        }
    }

    void jump(float deltaTime) {
        rb->upwardsSpeed = jumpPower;
        rb->useGravity();
        MovePlayer(deltaTime);

        glm::vec3 finalPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
        float jumpDistance = glm::length(finalPosition.y - initialPosition.y);
        if (jumpDistance > limitJump) {
            state = PlayerState::air;
            rb->upwardsSpeed = 0.f;
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimationWithY("jumping down", true);
        }
    }

    void checkState() {
        if (queueAnim.size() == 0) {
            animCount = 0;
        }
        if (input->checkAnyKey()) {
             if (state == PlayerState::climbing) {
                 if (input->checkKey(GLFW_KEY_W)) {
                     currentClimbingSpeed = climbingSpeed;
                 }
                 else if (input->checkKey(GLFW_KEY_S)) {
                     currentClimbingSpeed = -climbingSpeed;
                 }
                 else {
                     currentClimbingSpeed = 0.f;
                 }
            }
            else if (state != PlayerState::air && state != PlayerState::jump_up && state != PlayerState::climbing) {
                if (input->getPressKey() == GLFW_MOUSE_BUTTON_LEFT) {
                    if (queueAnim.size() < 3) {
                        queueAnim.push("attack" + queueAnim.size() + 1);
                    }
                    state = PlayerState::attack1;
                }
                if (input->checkKey(GLFW_MOUSE_BUTTON_RIGHT))
                {
                    state = PlayerState::dodge;
                }
                else if (input->checkKey(GLFW_KEY_SPACE) && !wasSpacePressed) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimationWithY("jumping up", true);
                    state = PlayerState::jump_up;
                    initialPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
                }
            }
            else {
                input->getPressKey();
            }
        }
        else if (state == PlayerState::walking) {
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("standing");
        }
        if (glm::distance(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y, initialPosition.y) < 0.5f && state == PlayerState::air) {
            state = PlayerState::walking;
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("standing");
            sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y = initialPosition.y;
        }
        wasSpacePressed = input->checkKey(GLFW_KEY_SPACE);
    }

public:
    
    PlayerMovement(SceneManager* sm, Input* input, Camera* camera, TimeManager* tm) {
        this->sm = sm;
        this->input = input;
        this->camera = camera;
        this->tm = tm;
        rb = new RigidBody("player", sm, tm);
    }

    void ManagePlayer(float& deltaTime2, float deltaTime) {
        //std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        this->deltaTime2 = deltaTime2;
        checkState();
        if (state == PlayerState::walking) {
            MovePlayer(deltaTime);
            // std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        }
        else if (state == PlayerState::attack1) {
            if (animCount == 0 && queueAnim.size()!= 0) {
                animCount++;
                sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("attack1");
            }
            move(deltaTime, false);
            if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
                if (queueAnim.size() > 1 && animCount == 1) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("attack2");
                    animCount++;
                }
                else if (queueAnim.size() > 2 && animCount == 2) {
                        animCount = 0;
                        while (!queueAnim.empty()) {
                            queueAnim.pop();
                        }
                        sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("attack3");
                }
                else {
                    state = PlayerState::walking;
                }
            }
        }
        else if (state == PlayerState::dodge) {
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("dodge");
            //currentTurn = 180.f;
            move(deltaTime, false);
            if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
                state = PlayerState::walking;
            }
        }
        else if(state == PlayerState::climbing)
        {
            if(currentClimbingSpeed > 0) {
                sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimationWithY("climbing up");
            }
            else if (currentClimbingSpeed < 0) {
                sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimationWithY("climbing down");
            }

            float climbingY = climbingSpeed * sm->getActiveScene()->findByName("player")->getTransform()->getLocalScale().y * sm->getActiveScene()->findByName("player")->getAnimateBody()->getPosition().y;
            sm->getActiveScene()->findByName("player")->velocity = glm::vec3(0.f, climbingY, 0.f) / deltaTime;
        }
        else if (state == PlayerState::jump_up) {
            jump(deltaTime);
        }
        else if (state == PlayerState::air) {
            MovePlayer(deltaTime);
            rb->useGravity();
        }
        if (state != PlayerState::attack1) {
            while (!queueAnim.empty()) {
                queueAnim.pop();
            }
        }
    }

    void changeState(PlayerState state) {
        this->state = state;
    }

    PlayerState getState() {
        return state;
    }

    ~PlayerMovement();
};

#endif
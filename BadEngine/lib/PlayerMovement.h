#ifndef PlayerMovement_H
#define PlayerMovement_H
#include <cmath>
#include "Transform.h"
#include "TimeManager.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class PlayerMovement {
private:
    SceneManager* sm;
    Input* input;
    Camera* camera;
    TimeManager* tm;
    RigidBody* rb;
    AnimateBody* animateBody;

    float moveSpeed = 4.f;
    float deltaTime2 = 0.f;
    float runSpeed = 10.f;
    float turnSpeed = 90.f;

    float currentTurn = 0.f;
    float rotateAngle = 0.f;

    float limitJump = 7.f;
    float jumpPower = 5.f;
    glm::vec3 initialPosition;
    float airSpeed = 5.f;

    enum State {
        walking,
        jump_up,
        air,
        attack1,
        dodge,
        climbing
    };
    State state = walking;

    bool wasSpacePressed = false;

    void getRotate() {
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
    }
    void move() {
        getRotate();
        float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
       // sm->getActiveScene()->findByName("player")->setRotating(false, angle + glm::radians(rotateAngle), glm::vec3(0.f, 1.f, 0.f));
        sm->getActiveScene()->findByName("player")->getTransform()->localRotation.y = angle + rotateAngle;

        float rotate = sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y;
        float sinRotate = sin(glm::radians(rotate));
        float cosRotate = cos(glm::radians(rotate));

        float dx = animateBody->getPosition().z * sinRotate + animateBody->getPosition().x * cosRotate;
        float dz = animateBody->getPosition().z * cosRotate - animateBody->getPosition().x * sinRotate;

        sm->getActiveScene()->findByName("player")->Move(glm::vec3(dx, 0.f, dz));
    }

    void moveInAir(float speed) {
        getRotate();
        float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
         sm->getActiveScene()->findByName("player")->getTransform()->localRotation.y = angle + rotateAngle;
        float distance = speed * tm->getFramePerSeconds();
        float dx = distance * sin(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        float dz = distance * cos(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        sm->getActiveScene()->findByName("player")->Move(glm::vec3(dx, 0.f, dz));
    }

    void MovePlayer() {
        float speed = 0.f;
        if (input->checkAnyKey() && deltaTime2 > 0.02f) {
            deltaTime2 = 0.f;
            if (input->checkKey(GLFW_KEY_LEFT_SHIFT) && state == walking) {
                // speed = runSpeed;
            }
            if (input->checkKey(GLFW_KEY_W)) {
                speed = airSpeed;
                if (state == walking) {
                    animateBody->setActiveAnimation("walking");
                }
                currentTurn = 180.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -135.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 135.f;
            }
            else if (input->checkKey(GLFW_KEY_S)) {
                speed = airSpeed;
                if (state == walking) {
                    animateBody->setActiveAnimation("walking");
                }
                currentTurn = 0.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -45.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 45.f;
            }
            else if (input->checkKey(GLFW_KEY_D)) {
                speed = airSpeed;
                if (state == walking) {
                    animateBody->setActiveAnimation("walking");
                }
                currentTurn = 90;
            }
            else if (input->checkKey(GLFW_KEY_A)) {
                speed = airSpeed;
                if (state == walking) {
                    animateBody->setActiveAnimation("walking");
                }
                currentTurn = -90;
            }
        }
        if (input->checkAnyKey()) {
            if (state == air || state == jump_up) {
                moveInAir(speed);
            }
            else
            {
                move();
            }
        }
    }

    void jump() {
        rb->upwardsSpeed = jumpPower;
        rb->useGravity();
        MovePlayer();

        glm::vec3 finalPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
        float jumpDistance = glm::length(finalPosition.y - initialPosition.y);
        if (jumpDistance > limitJump) {
            state = air;
            rb->upwardsSpeed = 0.f;
            animateBody->setActiveAnimation("jumping down");
        }
    }

    void checkState() {
        if (input->checkAnyKey()) {
            if (input->checkKey(GLFW_MOUSE_BUTTON_RIGHT))
            {
                state = dodge;
            }
            else if (input->checkKey(GLFW_KEY_SPACE) && state != air && state != jump_up && !wasSpacePressed) {
                animateBody->setActiveAnimation("jumping up");
                //animateBody->setActiveAnimation("standing");
                state = jump_up;
                initialPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
                // std::cout << glm::to_string(initialPosition) << std::endl;
            } 
            else if (input->checkKey(GLFW_MOUSE_BUTTON_LEFT)) {
                state = attack1;
            }
        }
        else if (state == walking) {
            animateBody->setActiveAnimation("standing");
        }
        if (glm::distance(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y, initialPosition.y) < 0.5f && state == air) {
            state = walking;
            animateBody->setActiveAnimation("standing");
            sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y = initialPosition.y;
            // std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
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

    void ManagePlayer(float& deltaTime2) {
        this->deltaTime2 = deltaTime2;
        checkState();
        if (state == walking) {
            MovePlayer();
            // std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        }
        else if (state == attack1) {
            animateBody->setActiveAnimation("attack1");
            if (animateBody->isPlay() == false) {
                state = walking;
            }
        }
        else if (state == dodge) {
            animateBody->setActiveAnimation("dodge");
            //currentTurn = 180.f;
            move();
            if (animateBody->isPlay() == false) {
                state = walking;
            }
        }
        else if (state == jump_up) {
            jump();
        }
        else if (state == air) {
            MovePlayer();
            rb->useGravity();
        }
    }

    void addAnimationPlayer(AnimateBody* ab) {
        animateBody = ab;
    }

    ~PlayerMovement();
};

#endif
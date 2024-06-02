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
    std::queue < std:: string > queueAnim;
    int animCount = 0;

    float moveSpeed = 4.f;
    float deltaTime2 = 0.f;
    float runSpeed = 10.f;
    float turnSpeed = 90.f;

    float currentTurn = 0.f;
    float rotateAngle = 0.f;

    float limitJump = 7.f;
    float jumpPower = 5.f;
    glm::vec3 initialPosition;
    float airSpeed = 4.f;
    bool firstSequence = true;

    enum State {
        walking,
        jump_up,
        air,
        attack1,
        attack2,
        attack3,
        dodge,
        climbing
    };
    State state = walking;

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
        sm->getActiveScene()->findByName("player")->velocity = vel;
    }

    void moveInAir(float speed) {
        getRotate(true);
        float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
         sm->getActiveScene()->findByName("player")->getTransform()->localRotation.y = angle + rotateAngle;
        float distance = speed * tm->getFramePerSeconds();
        float dx = distance * sin(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        float dz = distance * cos(glm::radians(sm->getActiveScene()->findByName("player")->getTransform()->getLocalRotation().y));
        sm->getActiveScene()->findByName("player")->Move(glm::vec3(dx, 0.f, dz));
    }

    void MovePlayer(float deltaTime) {
        float speed = 0.f;
        if (input->checkAnyKey() && deltaTime2 > 0.02f) {
            deltaTime2 = 0.f;
            if (input->checkKey(GLFW_KEY_LEFT_SHIFT) && state == walking) {
                // speed = runSpeed;
            }
            if (input->checkKey(GLFW_KEY_W)) {
                speed = airSpeed;
                if (state == walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 180.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -135.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 135.f;
            }
            else if (input->checkKey(GLFW_KEY_S)) {
                speed = airSpeed;
                if (state == walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 0.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -45.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 45.f;
            }
            else if (input->checkKey(GLFW_KEY_D)) {
                speed = airSpeed;
                if (state == walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 90;
            }
            else if (input->checkKey(GLFW_KEY_A)) {
                speed = airSpeed;
                if (state == walking) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
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
            state = air;
            rb->upwardsSpeed = 0.f;
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("jumping down", true);
        }
    }

    void checkState() {
        if (input->checkAnyKey()) {
            if (state != air && state != jump_up) {
                int key = input->getPressKey();
                if (key == GLFW_MOUSE_BUTTON_LEFT) {
                    if (queueAnim.size() < 3 && firstSequence == true) {
                        if (queueAnim.size() > 0 && queueAnim.front() != "attack1") {
                            if (queueAnim.front() == "attack2") {
                                queueAnim.push("attack" + std::to_string(queueAnim.size() + 2));
                            }
                            else {
                                queueAnim.push("attack" + std::to_string(queueAnim.size()));
                            }
                        }
                        else
                        {
                            queueAnim.push("attack" + std::to_string(queueAnim.size() + 1));
                        }
                    }
                    state = attack1;
                }
                if (input->checkKey(GLFW_MOUSE_BUTTON_RIGHT))
                {
                    state = dodge;
                }
                else if (input->checkKey(GLFW_KEY_SPACE) && !wasSpacePressed) {
                    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("jumping up", true);
                    state = jump_up;
                    initialPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
                }
            }
            else {
                input->getPressKey();
            }
        }
        else if (state == walking) {
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("standing");
        }
        if (glm::distance(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y, initialPosition.y) < 0.5f && state == air) {
            state = walking;
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
        if (state == walking) {
            MovePlayer(deltaTime);
            // std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        }
        else if (state == attack1) {
            std::cout << queueAnim.size()<<std::endl;
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation(queueAnim.front());
            if (queueAnim.front() == "attack3") {
                firstSequence = true;
            }
            else if (queueAnim.size() == 2 && queueAnim.front() == "attack2") {
                firstSequence = false;
            }
            if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
                if (queueAnim.size() == 3) {
                    firstSequence = false;
                }
                queueAnim.pop();
                if (queueAnim.size() > 1)
                {
                    //state = attack2;
                }
                if (queueAnim.size() == 0) {
                    firstSequence = true;
                    state = walking;
                }
            }

          /*  if (animCount == 0 && queueAnim.size()!= 0) {
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
                    state = walking;
                }
            }*/
        }
        //else if (state == attack2) {
        //        sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("attack2");
        //        if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
        //            if (queueAnim.size() > 2)
        //            {
        //                state = attack3;

        //                while (!queueAnim.empty()) {
        //                    queueAnim.pop();
        //                }
        //            }
        //            else {
        //                state = walking;
        //            }
        //        }
        //}
        //else if (state == attack3) {
        //    std::cout << "attack3";
        //    sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("attack3");
        //    if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
        //        if (queueAnim.size() > 1)
        //        {
        //            state = attack1;
        //        }
        //        else {
        //            state = walking;
        //        }
        //    }

        //}
        else if (state == dodge) {
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("dodge");
            //currentTurn = 180.f;
            move(deltaTime, false);
            if (sm->getActiveScene()->findByName("player")->getAnimateBody()->isPlay() == false) {
                state = walking;
            }
        }
        else if (state == jump_up) {
            jump(deltaTime);
        }
        else if (state == air) {
            MovePlayer(deltaTime);
            rb->useGravity();
        }
        if (state != attack1) {
            while (!queueAnim.empty()) {
                queueAnim.pop();
            }
        }
    }

    ~PlayerMovement();
};

#endif
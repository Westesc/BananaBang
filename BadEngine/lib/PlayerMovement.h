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
    climbing,
    tree_attack,
    leave_banana, 
    dashing,
    sprint
};

class PlayerMovement {
private:

    enum class PlayerStateAttack {
        none, 
        walking,
        sprint
    };

    //references
    SceneManager* sm;
    Input* input;
    Camera* camera;
    TimeManager* tm;
    RigidBody* rb;
    GameObject* player;
    PlayerMovement* pm = nullptr;

    //animation
    std::queue < std::string > queueAnim;
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
    float jumpPower = 7.f;
    glm::vec3 initialPosition;
    float airSpeed = 6.f;

    //climbing
    float climbingSpeed = 0.2f;
    float currentClimbingSpeed = 4.f;
    float groundPosition;
    glm::vec3 treePosition;
    float direction = 0.0f;

    //attack
    bool isMove = false;

    //treeAttack
    Enemy* closestEnemy;

    //dash
    float dashSpeed = 25.f;
    float dashDuration = 2000.f; 
    float currentDashTime = 0.f; 

    PlayerState state = PlayerState::walking;
    PlayerStateAttack attackState = PlayerStateAttack::none;

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
            player->getTransform()->localRotation.y = angle + rotateAngle;
        }
    }
    void move(float deltaTime, bool isRotate = true) {
        getRotate(isRotate);

        if (state != PlayerState::dashing) {
            float rotate = player->getTransform()->getLocalRotation().y;
            float sinRotate = sin(glm::radians(rotate));
            float cosRotate = cos(glm::radians(rotate));

            float dx = player->getTransform()->getLocalScale().x * player->getAnimateBody()->getPosition().z * sinRotate + player->getAnimateBody()->getPosition().x * cosRotate;
            float dz = player->getTransform()->getLocalScale().z * player->getAnimateBody()->getPosition().z * cosRotate - player->getAnimateBody()->getPosition().x * sinRotate;

            // sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.f, dy, 0.f));
            glm::vec3 vel = glm::vec3(dx, 0.f, dz) / deltaTime;
            player->velocity = vel * 2.0f;
        }
    }

    void moveInAir(float speed, float deltaTime) {
        getRotate(true);
        float angle = atan2(camera->getFront().x, camera->getFront().z) * (180.0 / M_PI);
        player->getTransform()->localRotation.y = angle + rotateAngle;
        float distance = speed * tm->getFramePerSeconds();
        float dx = distance * sin(glm::radians(player->getTransform()->getLocalRotation().y));
        float dz = distance * cos(glm::radians(player->getTransform()->getLocalRotation().y));
        //std::cout << "dx: " << dx / deltaTime << "dz: " << dz / deltaTime << std::endl;
        player->velocity.x = dx / deltaTime;
        player->velocity.z = dz / deltaTime;
    }

    void climbMove(float deltaTime) {
        static float radius = glm::distance(glm::vec2(player->getTransform()->getLocalPosition().x, player->getTransform()->getLocalPosition().z),
            glm::vec2(treePosition.x, treePosition.z)) - 0.4f;
        static float angle = std::atan2(player->getTransform()->getLocalPosition().z - treePosition.z, player->getTransform()->getLocalPosition().x - treePosition.x);

        float speed = direction * glm::pi<float>() / 2.0f;
        angle += speed * deltaTime;

        if (angle > 2 * glm::pi<float>()) {
            angle -= 2 * glm::pi<float>();
        }
        //std::cout << angle << std::endl;
        player->getTransform()->localRotation.y = -angle * (180.0 / M_PI) - 90.f;

        float x = treePosition.x + radius * std::cos(angle);
        float z = treePosition.z + radius * std::sin(angle);

        glm::vec3 newPosition = glm::vec3(x, player->getTransform()->getLocalPosition().y, z);
        player->getTransform()->localPosition = newPosition;
    }

    void MovePlayer(float deltaTime) {
        float speed = 0.f;
        if (input->checkAnyKey() && deltaTime2 > 0.02f) {
            deltaTime2 = 0.f;
            if (input->checkKey(GLFW_KEY_W)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    player->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 180.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -135.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 135.f;
            }
            else if (input->checkKey(GLFW_KEY_S)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    player->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 0.f;
                if (input->checkKey(GLFW_KEY_A)) currentTurn = -45.f;
                else if (input->checkKey(GLFW_KEY_D)) currentTurn = 45.f;
            }
            else if (input->checkKey(GLFW_KEY_D)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    player->getAnimateBody()->setActiveAnimation("walking");
                }
                currentTurn = 90;
            }
            else if (input->checkKey(GLFW_KEY_A)) {
                speed = airSpeed;
                if (state == PlayerState::walking) {
                    player->getAnimateBody()->setActiveAnimation("walking");
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

        glm::vec3 finalPosition = player->getTransform()->getLocalPosition();
        float jumpDistance = glm::length(finalPosition.y - initialPosition.y);
        if (jumpDistance == 0.0f) {
            state = PlayerState::walking;
            rb->upwardsSpeed = 0.f;
            sm->getActiveScene()->findByName("player")->getAnimateBody()->setActiveAnimation("walking");
        }
        if (jumpDistance > limitJump) {
            state = PlayerState::air;
            rb->upwardsSpeed = 0.f;
            //player->getAnimateBody()->setActiveAnimationWithY("jumping down", true);
        }
    }

    void rotatePlayerTowards(glm::vec3 targetPosition) {
        glm::vec3 playerPosition = player->getTransform()->getLocalPosition();
        glm::vec3 direction = glm::normalize(targetPosition - playerPosition);
        float angle = atan2(direction.x, direction.z);
        player->getTransform()->localRotation.y = glm::degrees(angle);
    }

    void checkState() {
        if (queueAnim.size() == 0) {
            animCount = 0;
        }
        if (input->checkAnyKey()) {
            if (state == PlayerState::attack1) {
                if (input->checkKey(GLFW_KEY_W) || input->checkKey(GLFW_KEY_A) || input->checkKey(GLFW_KEY_S) || input->checkKey(GLFW_KEY_D))
                {
                    attackState = PlayerStateAttack::walking;
                    if (input->checkKey(GLFW_KEY_LEFT_SHIFT)) {
                            attackState = PlayerStateAttack::sprint;
                    }
                }
                else if (attackState != PlayerStateAttack::none) {
                    attackState = PlayerStateAttack::none;
                }
            }
            if (state == PlayerState::climbing) {
                player->getTransform()->getLocalPosition().y;
                if (input->checkKey(GLFW_KEY_A)) {
                    direction = 0.3f;
                    currentClimbingSpeed = 0.0f;
                }
                else if (input->checkKey(GLFW_KEY_D)) {
                    direction = -0.3f;
                    currentClimbingSpeed = 0.0;
                }
                else {
                    direction = 0.f;
                }

                if (input->checkKey(GLFW_KEY_W)) {
                    currentClimbingSpeed = climbingSpeed;
                }
                else if (input->checkKey(GLFW_KEY_S)) {
                    currentClimbingSpeed = -climbingSpeed;
                }
                else if (input->checkKey(GLFW_KEY_E))
                {
                    state = PlayerState::tree_attack;
                }
                else if (input->checkKey(GLFW_KEY_SPACE))
                {
                    state = PlayerState::air;
                }
            }
            else if (state != PlayerState::air && state != PlayerState::jump_up && state != PlayerState::climbing) {
                if (input->getPressKey() == GLFW_MOUSE_BUTTON_LEFT) {
                    if (queueAnim.size() < 3) {
                        queueAnim.push("attack" + queueAnim.size() + 1);
                    }
                    state = PlayerState::attack1;
                    player->children.at(0)->active = true;
                    player->children.at(0)->getTransform()->localPosition = player->getTransform()->localPosition;
                }
                if (input->checkKey(GLFW_KEY_E) && state == PlayerState::walking) {
                    state = PlayerState::leave_banana;
                }
                else if (input->checkKey(GLFW_MOUSE_BUTTON_RIGHT))
                {
                    state = PlayerState::dodge;
                }
                else if (input->checkKey(GLFW_KEY_SPACE) && !wasSpacePressed) {
                    //player->getAnimateBody()->setActiveAnimationWithY("jumping up", true);
                    state = PlayerState::jump_up;
                    initialPosition = player->getTransform()->getLocalPosition();
                }
                else if(input->checkKey(GLFW_KEY_LEFT_SHIFT)) {
                    if (state != PlayerState::attack1) {
                        state = PlayerState::sprint;
                    }
                }
                else if (state == PlayerState::sprint) {
                    state = PlayerState::walking;
                }
                else if (input->checkKey(GLFW_KEY_Z) /* && state != PlayerState::dashing*/) {
                    state = PlayerState::dashing;
                    currentDashTime = 0.f;
                }
                else if (state == PlayerState::dashing){
                    state = PlayerState::walking;
                }
            }
            else {
                input->getPressKey();
                isMove = false;
            }
        }
        else if (state == PlayerState::walking) {
            player->getAnimateBody()->setActiveAnimation("standing");
        }
        else {
            direction = 0.f;
            currentClimbingSpeed = -0.02f;
            isMove = false;
        }
        if (glm::distance(player->getTransform()->getLocalPosition().y, groundPosition) < 0.5f && state == PlayerState::air) {
            state = PlayerState::walking;
            player->getAnimateBody()->setActiveAnimation("standing");
            player->getTransform()->localPosition.y = initialPosition.y;
        }
        else if (state != PlayerState::attack1)
        {
            player->getAnimateBody()->removeLegAnimation();
        }
        wasSpacePressed = input->checkKey(GLFW_KEY_SPACE);
        if (!(state == PlayerState::attack1 || state == PlayerState::tree_attack)) {
            player->children.at(0)->active = false;
        }
    }

public:

    PlayerMovement(SceneManager* sm, Input* input, Camera* camera, TimeManager* tm) {
        this->sm = sm;
        this->input = input;
        this->camera = camera;
        this->tm = tm;
        rb = new RigidBody("player", sm, tm);
        player = sm->getActiveScene()->findByName("player");
    }

    void ManagePlayer(float& deltaTime2, float deltaTime) {
        player = sm->getActiveScene()->findByName("player");
        player->useGravity = true;
        //std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        this->deltaTime2 = deltaTime2;
        PlayerState prevState = state;
        checkState();
        if (state == PlayerState::walking) {
            MovePlayer(deltaTime);
            // std::cout << sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y << std::endl;
        }
        else if (state == PlayerState::sprint) {
            player->getAnimateBody()->setActiveAnimation("sprint");
            MovePlayer(deltaTime);
        }
        else if (state == PlayerState::attack1) {
            if (attackState == PlayerStateAttack::walking) {
                player->getAnimateBody()->addLegAnimation("walking");
            }
            else if (attackState == PlayerStateAttack::sprint){
                player->getAnimateBody()->addLegAnimation("sprint");
            }
            else {
                player->getAnimateBody()->removeLegAnimation();
            }
            if (animCount == 0 && queueAnim.size() != 0 
                && (player->getAnimateBody()->getActiveAnimation() != "attack3" || player->getAnimateBody()->isPlay() == false)) {
                animCount++;
                player->getAnimateBody()->setActiveAnimation("attack1");
            }
            MovePlayer(deltaTime);
            if (player->getAnimateBody()->isPlay() == false) {
                if (queueAnim.size() > 1 && animCount == 1) {
                    player->getAnimateBody()->setActiveAnimation("attack2");
                    animCount++;
                }
                else if (queueAnim.size() > 2 && animCount == 2) {
                    animCount = 0;
                    while (!queueAnim.empty()) {
                        queueAnim.pop();
                    }
                    player->getAnimateBody()->setActiveAnimation("attack3");
                }
                else {
                    state = PlayerState::walking;
                }
            }
        }
        else if (state == PlayerState::dodge) {
            player->getAnimateBody()->setActiveAnimation("dodge");
            //currentTurn = 180.f;
            move(deltaTime, false);
            if (player->getAnimateBody()->isPlay() == false) {
                state = PlayerState::walking;
            }
        }
        else if (state == PlayerState::climbing)
        {
            player->useGravity = false;
            climbMove(deltaTime);
            if (currentClimbingSpeed > 0) {
                player->getAnimateBody()->setActiveAnimationWithY("climbing up");
                if (currentClimbingSpeed == -0.02f) {
                    player->getAnimateBody()->changeAnimationSpeed("climbing up", 0.6f);
                }
                else {
                    player->getAnimateBody()->changeAnimationSpeed("climbing up", 1.3f);
                }
            }
            else if (currentClimbingSpeed < -0.02f) {
                player->getAnimateBody()->setActiveAnimationWithY("climbing down");
                if (groundPosition + 0.05 > player->getTransform()->getLocalPosition().y) {
                    state = PlayerState::walking;
                }
            }

            //float climbingY = abs(currentClimbingSpeed) * player->getTransform()->getLocalScale().y * player->getAnimateBody()->getPosition().y;
            //std::cout << climbingY << std::endl;
            //player->Move(glm::vec3(0.f, currentClimbingSpeed * deltaTime, 0.f));
            player->velocity.y = currentClimbingSpeed / deltaTime;
        }
        else if (state == PlayerState::tree_attack) {
            //Jakiœ warunek co bêdzie sprawdza³ czy œcie¿ka jest git
            if (closestEnemy != nullptr) {
                player->useGravity = false;
                if (closestEnemy->getTransform()) {
                    rotatePlayerTowards(closestEnemy->getTransform()->getLocalPosition());

                    //player->getAnimateBody()->setActiveAnimation("tree attack");

                    glm::vec3 currentPosition = player->getTransform()->getLocalPosition();
                    glm::vec3 direction = glm::normalize(closestEnemy->getTransform()->getLocalPosition() - currentPosition);

                    float speed = 1.5f;
                    direction.y -= 0.05;
                    glm::vec3 velocity = direction * speed;

                    player->velocity = velocity / deltaTime;
                    player->children.at(0)->active = true;
                    player->children.at(0)->getTransform()->localPosition = player->getTransform()->localPosition;
                }
                if (groundPosition + 0.05 > player->getTransform()->getLocalPosition().y) {
                    state = PlayerState::walking;
                    player->children.at(0)->active = false;
                }
            }
            else {
                state = PlayerState::climbing;
            }
        }
        else if (state == PlayerState::jump_up) {
            jump(deltaTime);
        }
        else if (state == PlayerState::air) {
            MovePlayer(deltaTime);
            rb->upwardsSpeed = -7.f;
            rb->useGravity();
        }
        else if (state == PlayerState::leave_banana) {

            //sm->getActiveScene()->findByName("banana")->getTransform()->localPosition = glm::vec3(player->getTransform()->getLocalPosition().x, 0.2f, player->getTransform()->getLocalPosition().z);
            state = PlayerState::walking;
            //if (player->getAnimateBody()->getActiveAnimation() != "leave banana up") {
            //    player->getAnimateBody()->setActiveAnimation("leave banana down", true);
            //}
            //if (player->getAnimateBody()->isPlay() == false && player->getAnimateBody()->getActiveAnimation() == "leave banana down") {
            //    sm->getActiveScene()->findByName("banana")->getTransform()->localPosition = glm::vec3(player->getTransform()->getLocalPosition().x, 0.2f, player->getTransform()->getLocalPosition().z);
            //    player->getAnimateBody()->setActiveAnimation("leave banana up", true);
            //}
            //else if (player->getAnimateBody()->isPlay() == false && player->getAnimateBody()->getActiveAnimation() == "leave banana up") {
            //    state = PlayerState::walking;
            //}
        }
        else if (state == PlayerState::dashing) {
            if (currentDashTime < dashDuration) {
                MovePlayer(deltaTime);
                float rotate = glm::radians(player->getTransform()->getLocalRotation().y);
                glm::vec3 dashDirection;
                dashDirection.x = sin(rotate);
                dashDirection.z = cos(rotate);
                dashDirection.y = 0;
                currentDashTime += deltaTime;
                //std::cout << "dush" << std::endl;
                player->velocity = glm::normalize(dashDirection) * dashSpeed;
            }
            else {
                //std::cout << "nie dush" << std::endl;
                state = PlayerState::walking;
            }
        }
        if (state != PlayerState::attack1) {
            while (!queueAnim.empty()) {
                queueAnim.pop();
            }
        }
    }

    void setTreePosition(glm::vec3 treePos) {
        treePosition = treePos;
    }

    void setAttackDestination(Enemy* enemy) {
        closestEnemy = enemy;
    }

    void changeState(PlayerState state) {
        this->state = state;
    }

    void setGroundPosition(float newGround) {
        groundPosition = newGround;
    }

    PlayerState getState() {
        return state;
    }

    Input* getInput() {
        return input;
    }

    ~PlayerMovement();
};

#endif
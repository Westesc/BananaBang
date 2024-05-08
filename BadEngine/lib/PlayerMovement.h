#ifndef PlayerMovement_H
#define PlayerMovement_H
#include <cmath>
#include "Transform.h"
#include "TimeManager.h"

class PlayerMovement {
private:
	SceneManager* sm;
	Input* input;
	Camera* camera;
	TimeManager* tm;
	RigidBody* rb;

	//movement
	float moveSpeed = 4.f;
	float deltaTime2 = 0.f;
	float runSpeed = 10.f;
	float turnSpeed = 90.f;

	//nie dotykaæ
	//float currentSpeed = 0.f;
	float currentTurn = 0.f;
	float rotateAngle = 0.f;

	//jumping
	float limitJump = 2.f;
	float jumpPower = 30.f;
	glm::vec3 initialPosition;
	float airSpeed = 5.f;

	//state
	enum State {
		walking,
		jump_up,
		air,
		climbing
	};
	State state = walking;
	void getRotate() 
	{
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

	void move(float speed) {
		getRotate();
		float angle = atan2(camera->getFront().x, camera->getFront().z);
		sm->getActiveScene()->findByName("player")->setRotating(false, angle + glm::radians(rotateAngle), glm::vec3(0.f, 1.f, 0.f));

		float distance = -speed * tm->getFramePerSeconds();
		float dx = distance * sin(sm->getActiveScene()->findByName("player")->getRotate());
		float dz = distance * cos(sm->getActiveScene()->findByName("player")->getRotate());
		sm->getActiveScene()->findByName("player")->Move(glm::vec3(dx, 0.0f, dz));
	}

	void MovePlayer(float speed)
	{
		if (input->checkAnyKey() && deltaTime2 > 0.02f)
		{
			deltaTime2 = 0.f;
			if (input->checkKey(GLFW_KEY_LEFT_SHIFT) && state == walking)
			{
				speed = runSpeed;
			}
			if (input->checkKey(GLFW_KEY_W))
			{
				currentTurn = 0.f;
			}
			else if (input->checkKey(GLFW_KEY_S))
			{
				currentTurn = 180.f;
			}
			if (input->checkKey(GLFW_KEY_D))
			{
				currentTurn = -90;
			}
			else if (input->checkKey(GLFW_KEY_A))
			{
				currentTurn = 90;
			}
			if (input->checkSequence(GLFW_KEY_1, GLFW_KEY_2)) {
				std::cout << "Wykryto sekwencje!" << std::endl;
			}
		}
		if (input->checkAnyKey()) {
			move(speed);
		}
	}

	void jump() {
		rb->upwardsSpeed = jumpPower;
		rb->useGravity();
		MovePlayer(airSpeed);

		glm::vec3 finalPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
		float jumpDistance = glm::length(finalPosition.y - initialPosition.y);
		if (jumpDistance > limitJump) {
			state = air;
		}
	}


	void checkState() {
		if (input->checkAnyKey()) {
			if (input->checkKey(GLFW_KEY_SPACE) && state != air && state != jump_up)
			{
				state = jump_up;
				initialPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
			}
		}
		//tyczasowo jeœli kolizuje z czymœ o tagu ground do wy³¹cz grawitacje
		if (glm::length(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y - initialPosition.y) < 0.5f && state == air) {
			state = walking;
			sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y = initialPosition.y;
		}
	}

public:
	PlayerMovement(SceneManager* sm, Input* input, Camera* camera, TimeManager* tm) {
		this->sm = sm;
		this->input = input;
		this->camera = camera;
		this->tm = tm;
		rb = new RigidBody("player", sm, tm);
	}

	void ManagePlayer(float& deltaTime2)
	{
		this->deltaTime2 = deltaTime2;
		checkState();
		if (state == walking) {
			MovePlayer(moveSpeed);
		}
		else if (state == jump_up) {
			jump();
		}
		else if (state == air) {
			MovePlayer(airSpeed);
			rb->useGravity();
		}
	}

	~PlayerMovement();

};

#endif
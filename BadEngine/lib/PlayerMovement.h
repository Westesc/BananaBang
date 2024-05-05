#ifndef PlayerMovement_H
#define PlayerMovement_H
#include <cmath>
#include "Transform.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
//djsfoijdkaopkd

class PlayerMovement {
private:
	SceneManager* sm;
	Input* input;
	Camera* camera;
	float time = 0.0f;

	//movement
	float moveSpeed = 4.f;
	float deltaTime2 = 0.f;
	float runSpeed = 10.f;
	float turnSpeed = 90.f;

	//nie dotykaæ
	float currentSpeed = 0.f;
	float currentTurn = 0.f;
	float rotateAngle = 0.f;

	//jumping
	float limitJump = 5.f;
	float jumpPower = 30.f;
	float gravity = -50.f;
	glm::vec3 initialPosition;
	float airSpeed = 5.f;

	float upwardsSpeed = 0.f;

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
		float rotationAmount = rotationSpeed * getFramePerSeconds();

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

		float distance = currentSpeed * getFramePerSeconds();
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
			if (input->checkKey(GLFW_KEY_R))
			{
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, -speed * getFramePerSeconds(), 0.0f));
			}
			if (input->checkKey(GLFW_KEY_E))
			{
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, speed * getFramePerSeconds(), 0.0f));
			}
			if (input->checkKey(GLFW_KEY_W))
			{
				currentSpeed = -speed;
				currentTurn = 0.f;
			}
			else if (input->checkKey(GLFW_KEY_S))
			{
				currentSpeed = -speed;
				currentTurn = 180.f;
			}
			if (input->checkKey(GLFW_KEY_D))
			{
				currentSpeed = -speed;
				currentTurn = -90;
			}
			else if (input->checkKey(GLFW_KEY_A))
			{
				currentSpeed = -speed;
				currentTurn = 90;
			}
			if (input->checkSequence(GLFW_KEY_1, GLFW_KEY_2)) {
				std::cout << "Wykryto sekwencje!" << std::endl;
			}
		}
		else if(!input->checkAnyKey()) 
		{
			currentSpeed = 0.f;
			//currentTurnSpeed = 0.f;
			//rotateAngle = 0.f;
		}
		if (input->checkAnyKey()) {
			move(speed);
		}
	}

	void jump() {
		useGravity();
		MovePlayer(airSpeed);
		glm::vec3 finalPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
		float jumpDistance = glm::length(finalPosition - initialPosition);
		if (jumpDistance > limitJump) {
			state = air;
		}
	}

	float getFramePerSeconds() {
		return time;
	}
	void setTime(float newTime) {
		time = newTime;
	}

	void useGravity() {
		MovePlayer(airSpeed);
		upwardsSpeed += gravity * getFramePerSeconds();
		sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, upwardsSpeed * getFramePerSeconds(), 0.0f));
	}

	void checkState() {
		if (input->checkAnyKey()) {
			if (input->checkKey(GLFW_KEY_SPACE) && state != air && state != jump_up)
			{
				upwardsSpeed = jumpPower;
				state = jump_up;
				initialPosition = sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition();
			}
		}
		//tyczasowo jeœli kolizuje z czymœ o tagu ground do wy³¹cz grawitacje
		if (glm::length(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y - initialPosition.y) < 0.5f && state == air) {
			state = walking;
			upwardsSpeed = 0.f;
			sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y = initialPosition.y;
		}
	}

public:
	PlayerMovement(SceneManager* sm, Input* input, Camera* camera) {
		this->sm = sm;
		this->input = input;
		this->camera = camera;
	}

	void ManagePlayer(float deltaTime, float& deltaTime2)
	{
		this->deltaTime2 = deltaTime2;
		//std::cout<<camera->transform->getLocalRotation().x<<", " << camera->transform->getLocalRotation().y <<", " << camera->transform->getLocalRotation().z <<std::endl;
		checkState();
		setTime(deltaTime);
		if (state == walking) {
			MovePlayer(moveSpeed);
		}
		else if (state == jump_up) {
			jump();
		}
		else if (state == air) {
			useGravity();
		}
	}

	~PlayerMovement();

};

#endif
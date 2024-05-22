#ifndef PlayerMovement_H
#define PlayerMovement_H
class PlayerMovement {
private:
	SceneManager* sm;
	Input* input;
	float time = 0.0f;

	//movement
	float moveSpeed = 4.f;
	float deltaTime2 = 0.f;
	float runSpeed = 10.f;
	float turnSpeed = 10.f;

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
	void MovePlayer(float speed)
	{
		ZoneScopedN("MovePlayer");
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
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, 0.0f, -speed * getFramePerSeconds()));
			}
			if (input->checkKey(GLFW_KEY_S))
			{
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(0.0f, 0.0f, speed * getFramePerSeconds()));
			}
			if (input->checkKey(GLFW_KEY_A))
			{
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(-speed * getFramePerSeconds(), 0.0f, 0.0f));
			}
			if (input->checkKey(GLFW_KEY_D))
			{
				sm->getActiveScene()->findByName("player")->Move(glm::vec3(speed * getFramePerSeconds(), 0.0f, 0.0f));
			}
			if (input->checkSequence(GLFW_KEY_1, GLFW_KEY_2)) {
				std::cout << "Wykryto sekwencje!" << std::endl;
			}
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
		std::cout << glm::length(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y - initialPosition.y) << std::endl;
		if (glm::length(sm->getActiveScene()->findByName("player")->getTransform()->getLocalPosition().y - initialPosition.y) < 0.5f && state == air) {
			state = walking;
			upwardsSpeed = 0.f;
			sm->getActiveScene()->findByName("player")->getTransform()->localPosition.y = initialPosition.y;
		}
	}

public:
	PlayerMovement(SceneManager* sm, Input* input) {
		this->sm = sm;
		this->input = input;
	}

	void ManagePlayer(float deltaTime, float& deltaTime2)
	{
		this->deltaTime2 = deltaTime2;
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
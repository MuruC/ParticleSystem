#pragma once
#include <vector>
#include "glCore.h"
#include "shader.h"
#include "camera.h"

struct ParticleProps
{
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	glm::vec4 colorBegin;
	glm::vec4 colorEnd;
	float size;
	float lifeTime = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void Emit(const ParticleProps& particleProps);
	void OnRender(Camera& camera);
	void OnUpdate(float ts);
private:
	struct Particle
	{
		glm::vec2 position;
		glm::vec2 velocity;
		glm::vec2 acceleration;
		glm::vec4 colorBegin;
		glm::vec4 colorEnd;
		float size;
		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f;
		bool active = false;
	};
	void RegisterParticleShader();
	Shader* particleShader = nullptr;
	std::vector<Particle> particlePool;
	uint32_t poolIndex = 999;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int modelLoc = 0;
	unsigned int viewProjectionLoc = 0;
};
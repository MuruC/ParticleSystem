#pragma once
#include <vector>
#include "glCore.h"
#include "shader.h"
#include "camera.h"

struct ParticleProps
{
	glm::vec2 position;
	glm::vec2 velocity = {0.0f, 0.0f};
	glm::vec2 acceleration = {3.0f, 1.0f};
	glm::vec4 colorBegin = { 1.0f, 0.5f, 0.2f, 1.0f };
	glm::vec4 colorEnd = { 1.0f, 0.1f, 0.05f, 0.05f };
	float sizeBegin = 1.0f;
	float sizeEnd = 0.3f;
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
		glm::vec4 currentColor;
		float rotation = 0.0f;
		float sizeBegin;
		float sizeEnd;
		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f;
		bool active = false;
	};
	void RegisterParticleShader();
	Shader* particleShader = nullptr;
	std::vector<Particle> particlePool;
	Particle* nonTransparentParticles[1000] = {};
	Particle* transparentParticles[1000] = {};
	uint32_t poolIndex = 999;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	unsigned int modelLoc = 0;
	unsigned int viewProjectionLoc = 0;
	unsigned int colorLoc = 0;
	void SortAlphaArray(Particle** nums, int arrayLen);
	int Partrition(Particle** nums, int l, int h);
	void QuickSort(Particle** nums, int l, int h);
};
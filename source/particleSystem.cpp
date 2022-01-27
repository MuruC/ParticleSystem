#include "particleSystem.h"
#include <iostream>
#include <vector>

ParticleSystem::ParticleSystem()
{
	particlePool.resize(1000);
}
ParticleSystem::~ParticleSystem()
{
    if (particleShader != nullptr)
        delete particleShader;
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
void ParticleSystem::RegisterParticleShader()
{
    if (particleShader != nullptr)
        delete particleShader;
	particleShader = new Shader("../../shader/shaders.vs", "../../shader/shaders.fs");
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
	Particle& particle = particlePool[poolIndex];
	particle.active = true;
    particle.lifeRemaining = particleProps.lifeTime;
    particle.rotation = GLCoreUtil::randomFloat() * 2.0f * glm::pi<float>();
	particle.position = particleProps.position;
    particle.acceleration = particleProps.acceleration;
    particle.sizeBegin = particleProps.sizeBegin;
    particle.sizeEnd = particleProps.sizeEnd;
    particle.velocity.x = (GLCoreUtil::randomFloat() - 0.5f) * particle.acceleration.x;
    particle.velocity.y = (GLCoreUtil::randomFloat() - 0.5f) * particle.acceleration.y;
    particle.colorBegin = particleProps.colorBegin;
    particle.colorEnd = particleProps.colorEnd;
	poolIndex = -- poolIndex % particlePool.size();
}

void ParticleSystem::OnUpdate(float ts)
{
    for (auto& particle:particlePool)
    {
        if (!particle.active)
            continue;
        particle.lifeRemaining -= ts;
        if (particle.lifeRemaining <= 0)
        {
            particle.active = false;
            continue;
        }
        particle.position += particle.velocity * ts;
        particle.rotation += 0.01f * ts;
    }
}

void ParticleSystem::OnRender(Camera& camera)
{
	if (!VAO)
	{
        // origin square
        float vertices[] = {
            // position        
            0.1f,  0.1f, 0.5f,
            0.1f, -0.1f, 0.5f,
           -0.1f, -0.1f, 0.5f,
           -0.1f,  0.1f, 0.5f,
        };
        unsigned int indices[] =
        {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // build and compile our shader program
        // ------------------------------------
        RegisterParticleShader();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        modelLoc = glGetUniformLocation(particleShader->ID, "model");
        viewProjectionLoc = glGetUniformLocation(particleShader->ID, "viewProjection");
        colorLoc = glGetUniformLocation(particleShader->ID, "particleColor");
	}
    particleShader->use();
    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix));
    int transParentIndex = 0;
    int nonTransParentIndex = 0;
    for (auto& particle : particlePool)
    {
        if (!particle.active)
            continue;
        float life = particle.lifeRemaining / particle.lifeTime;
        particle.currentColor = particle.colorEnd + life * (particle.colorBegin - particle.colorEnd);

        ParticleSystem::Particle* p = &particle;
        if (particle.currentColor.a >= 1)
        {
            nonTransparentParticles[nonTransParentIndex] = p;
            ++nonTransParentIndex;
        }
        else if (particle.currentColor.a < 1)
        {
            transparentParticles[transParentIndex] = p;
            ++transParentIndex;
        }
    }
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    for (int i = 0; i <= nonTransParentIndex; i++)
    {
        ParticleSystem::Particle* p = nonTransparentParticles[i];
        if (p == nullptr)
        {
            continue;
        }
        // Fade away particles
        float life = p->lifeRemaining / p->lifeTime;
        float size = std::lerp(p->sizeEnd, p->sizeBegin, life);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), { p->position.x, p->position.y, 0.0f })
            * glm::rotate(glm::mat4(1.0f), p->rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size, size, 1.0f });

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(colorLoc, 1, glm::value_ptr(p->currentColor));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    SortAlphaArray(transparentParticles, transParentIndex);
    for (int j = transParentIndex; j >= 0; --j)
    {
        ParticleSystem::Particle* p = transparentParticles[j];
        if (p == nullptr)
        {
            continue;
        }
        // Fade away particles
        float life = p->lifeRemaining / p->lifeTime;
        float size = std::lerp(p->sizeEnd, p->sizeBegin, life);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), { p->position.x, p->position.y, 0.0f })
            * glm::rotate(glm::mat4(1.0f), p->rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size, size, 1.0f });

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(colorLoc, 1, glm::value_ptr(p->currentColor));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void ParticleSystem::SortAlphaArray(Particle** nums, int arrayLen) {
    ParticleSystem::QuickSort(nums, 0, arrayLen);
}

int ParticleSystem::Partrition(Particle** nums, int l, int h)
{
    if (l >= h || nums == nullptr || nums[l] == nullptr)
    {
        return -1;
    }
    int pivot = nums[l]->currentColor.a;
    int left = l + 1;
    int right = h;
    while (left <= right)
    {
        if (nums[right] == nullptr || nums[left] == nullptr)
        {
            return -1;
        }
        if (nums[right]->currentColor.a > pivot)
        {
            right--;
        }
        else if (nums[left]->currentColor.a <= pivot)
        {
            left++;
        }
        else
        {
            std::swap(nums[left], nums[right]);
        }
    }
    std::swap(nums[l], nums[right]);
    return right;
}

void ParticleSystem::QuickSort(Particle** nums, int l, int h)
{
    if (l > h || nums == nullptr)
    {
        return;
    }
    int j = ParticleSystem::Partrition(nums, l, h);
    if (j != -1)
    {
        QuickSort(nums, l, j - 1);
        QuickSort(nums, j + 1, h);
    }
}
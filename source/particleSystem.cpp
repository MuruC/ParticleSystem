#include "particleSystem.h"
#include <iostream>

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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    for (auto& particle : particlePool)
    {
        if (!particle.active)
            continue;

        // Fade away particles
        float life = particle.lifeRemaining / particle.lifeTime;
        float size = std::lerp(particle.sizeEnd, particle.sizeBegin, life);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), { particle.position.x, particle.position.y, 0.0f }) 
                        * glm::scale(glm::mat4(1.0f), {size, size, 1.0f});
        glm::vec4 color = particle.colorEnd + life * (particle.colorBegin - particle.colorEnd);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glutUtil.h"
#include "particleSystem.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
glm::vec3 getViewPos(glm::mat4 pro, glm::mat4 view);
void drawLine(glm::vec3 endPos, Shader shader);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// mouse
float curMouseX = lastX;
float curMouseY = lastY;
bool clickMouse = false;

//particle props
ParticleProps particleProps;
ParticleSystem* particleSystem = new ParticleSystem;

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

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float mouseX = (float)curMouseX / (SCR_WIDTH * 0.5f) - 1.0f;
        float mouseY = 1.0f - curMouseY / (SCR_HEIGHT * 0.5f);
        particleProps.position.x = mouseX;
        particleProps.position.y = mouseY;

        particleSystem->OnRender(camera);

        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deletes all ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    //delete particleSystem;
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    curMouseX = lastX;
    curMouseY = lastY;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && GLFW_PRESS == action)
    {
        particleSystem->Emit(particleProps);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 getViewPos(glm::mat4 pro, glm::mat4 view)
{
    float mouseX = (float)curMouseX / (SCR_WIDTH * 0.5f) - 1.0f;
    float mouseY = curMouseY / (SCR_HEIGHT * 0.5f) - 1.0f;

    GLint viewPort[4] = { 0, 0, SCR_WIDTH, SCR_HEIGHT };

    GLfloat winX, winY, winZ;
    double objectX = 0.0, objectY = 0.0, objectZ = 0.0;
    winX = (float)curMouseX;
    winY = (float)viewPort[3] - (float)curMouseY - 1.0f;
    glm::vec3 screenPos = glm::vec3(winX, winY, 0.1f);
    glm::vec3 worldPos = glm::vec3(objectX, objectY, objectZ);
    glReadBuffer(GL_BACK);
    glReadPixels(int(winX),int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    glutUtil::glhUnProjectVecf(winX, winY, winZ, glm::value_ptr(view), glm::value_ptr(pro), viewPort, glm::value_ptr(worldPos));
    return worldPos;
}

void drawLine(glm::vec3 endPos, Shader shader)
{
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    double lineVertices[] = { cameraPos[0], cameraPos[1], cameraPos[2], endPos[0], endPos[1], endPos[2] };
    unsigned lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    // retrieve the matrix uniform locations
    unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
    // pass them to the shaders (3 different ways)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", model);

    glDrawArrays(GL_LINE_STRIP, 0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

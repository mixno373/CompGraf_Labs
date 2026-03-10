// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define GLEW_DLL
#define GLFW_DLL

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GrafShaders.h"

#include "Model.h"



const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float fov = 45.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
float sensitivity = 0.1f;

float delta_time = 0.0f;
float last_frame = 0.0f;

float move_head_v = 0.0f;
float move_head_h = 0.0f;
float rotate_shpind = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;


    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = delta_time * 2.0f;
    float rotSpeed = delta_time * 250.0f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        float newMove = move_head_v + moveSpeed;
        if (newMove <= 0.51f) {
            move_head_v = newMove;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        float newMove = move_head_v - moveSpeed;
        if (newMove >= -1.67f) {
            move_head_v = newMove;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        float newMove = move_head_h + moveSpeed;
        if (newMove <= 0.37f) {
            move_head_h = newMove;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        float newMove = move_head_h - moveSpeed;
        if (newMove >= -1.6f) {
            move_head_h = newMove;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        float newRot = rotate_shpind + rotSpeed;

        if (newRot <= 360.0f) {
            rotate_shpind = newRot;
        }
        else {
            rotate_shpind = -360.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        float newRot = rotate_shpind - rotSpeed;

        if (newRot >= -360.0f) {
            rotate_shpind = newRot;
        }
        else {
            rotate_shpind = 360.0f;
        }
    }
}

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "4PU 900VH", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;

    GLenum ret = glewInit();
    if (ret != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(ret));
        return 1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    Shader* shader = new Shader();
    if (shader->load("vert_shader.glsl", "frag_shader.glsl") == 0) {
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    Model ourModel("4PU.obj");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();

        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;


        processInput(window);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        unsigned int modelLoc = glGetUniformLocation(shader->shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shader->shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shader->shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glUniform3f(glGetUniformLocation(shader->shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "material.ambient"), 0.3f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "material.diffuse"), 0.3f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "material.specular"), 0.15f, 1.0f, 0.5f);
        glUniform1f(glGetUniformLocation(shader->shaderProgram, "material.shininess"), 32.0f);

        glUniform3f(glGetUniformLocation(shader->shaderProgram, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shader->shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        glm::mat4 modelMatrices[4];

        modelMatrices[0] = glm::mat4(1.0f);

        modelMatrices[1] = glm::mat4(1.0f);

        modelMatrices[2] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, move_head_v, 0.0f));

        glm::vec3 pivot(-0.0834995f, 1.961549f, 0.2648f);
        glm::mat4 spindle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, move_head_v, 0.0f));
        spindle = glm::translate(spindle, glm::vec3(0.0f, 0.0f, move_head_h));
        spindle = glm::translate(spindle, pivot);
        spindle = glm::rotate(spindle, glm::radians(rotate_shpind), glm::vec3(0.0f, 1.0f, 0.0f));
        spindle = glm::translate(spindle, -pivot);
        modelMatrices[3] = spindle;

        ourModel.Draw(*shader, modelMatrices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader->shaderProgram);
    glfwTerminate();

    return 0;
}
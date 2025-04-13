#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "HUD.h"
#include "Graphics.h"

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Game
{
public:
	Game()
	{
        glfwSetErrorCallback(glfw_error_callback);

        if (!glfwInit())
            std::cerr << "Failed to initialize GLFW";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 3);

        window = glfwCreateWindow(800, 800, "Minecrap", NULL, NULL);

        glfwMakeContextCurrent(window);
        

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
        std::cout << "all g";
        tManager = TextureManager();
        tManager.Init();
        sManager = ShaderManager(1);

        glEnable(GL_MULTISAMPLE);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, 800, 800);



        crosshair = Crosshair::Crosshair(1);
        hotbar = Hotbar::Hotbar(1);
        std::cout << sManager.texmmLoc << "\n";
        planet = new Planet(5, sManager.texmmLoc, time(NULL));

        glClearColor(0.639f, 0.8f, 0.984f, 1.0f);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        deltaTime = 0.0f;
        prevTime = glfwGetTime();

	}
    void GameLoop()
    {
        while (!glfwWindowShouldClose(window)) {
            // Clear buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            double currentTime = glfwGetTime();
            deltaTime = currentTime - prevTime;
            prevTime = currentTime;

            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, tManager.crosshairTexture.ID);
            crosshair.Render(sManager.invertedShaderProgram);

            hotbar.Render(sManager.invertedShaderProgram, float(camera.selectedBlock));

            glBindTexture(GL_TEXTURE_2D, tManager.atlas1.ID);

            glUseProgram(sManager.textureShaderProgram);

            camera.Inputs(window, deltaTime, planet);
            camera.Matrix(45.0f, 0.1f, 500.0f, sManager.textureShaderProgram, "camMatrix");

            planet->Update(camera, sManager.texmmLoc);
            planet->Render();

            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }
    void Terminate()
    {
        glfwTerminate();
    }

private:
    GLFWwindow* window;
    Camera camera = Camera::Camera(800, 800, glm::vec3(0.0f, 68.0f, 1.0f));
    Planet* planet;
    Crosshair crosshair;
    Hotbar hotbar;
    TextureManager tManager;
    ShaderManager sManager;
    double deltaTime, prevTime;
};
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "HUD.h"
#include "Graphics.h"

//  Callback for handling GLFW errors
void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
//  Callback for window resizes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//  The main game class
class Game
{
public:
	Game()
	{
        glfwSetErrorCallback(glfw_error_callback); // Implement the callback

        if (!glfwInit())
            std::cerr << "Failed to initialize GLFW"; // Notify of GLFW failure
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //   Window hints (Version x., Version .x, profile)
        glfwWindowHint(GLFW_SAMPLES, 3); // Implementation of mild antialiasing

        window = glfwCreateWindow(800, 800, "Minecrap", NULL, NULL); // Create a window 800x800

        glfwMakeContextCurrent(window); //  Critical: creating an OpenGL context
        

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window);
            glfwTerminate(); // Exit if glad (basically OpenGL) failed to load
        }
        std::cout << "all g"; // Old debug message, still nice to have
        tManager = TextureManager(); // Creating the texture manager
        tManager.Init(); // This funcion exists to prevent throws
        sManager = ShaderManager(1); //Creating the shaderManager

        glEnable(GL_MULTISAMPLE); //    Enabling antialiasing
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //   Enabling window resize

        glEnable(GL_DEPTH_TEST); // Enabling depth tests for no weird visual errors
        glViewport(0, 0, 800, 800); // The viewport will be 800x800px



        crosshair = Crosshair::Crosshair(1); // Creating the crosshair using a dummy constructor
        hotbar = Hotbar::Hotbar(1); // Creating the hotbar using a dummy constructor
        planet = new Planet(5, sManager.texmmLoc, time(NULL)); //   Critical: creating the planet

        glClearColor(0.639f, 0.8f, 0.984f, 1.0f); // Sky colour
        glDisable(GL_CULL_FACE); // Transparency stuff
        glEnable(GL_BLEND); // More Transparency stuff
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Even more transparency stuff


        deltaTime = 0.0f; // This will be used for calculating things like movement per second. Every single unit dependant on time will now be per second.
        prevTime = glfwGetTime(); // Current time

	}
    void GameLoop()
    {
        while (!glfwWindowShouldClose(window)) { // The game loop, continue until the window should close.
            // Clear buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the screen and the depth buffer

            double currentTime = glfwGetTime(); // Setting the current time
            deltaTime = currentTime - prevTime; // Calculating the time difference
            prevTime = currentTime; // Setting prevTime to the current time for future calculations

            glActiveTexture(GL_TEXTURE0); // Activating the textures

            glBindTexture(GL_TEXTURE_2D, tManager.crosshairTexture.ID);  // Binding the crosshair texture from the manager
            crosshair.Render(sManager.invertedShaderProgram); // Rendering the crosshair

            hotbar.Render(sManager.invertedShaderProgram, float(camera.selectedBlock), &camera); // Rendering the hotbar

            glBindTexture(GL_TEXTURE_2D, tManager.atlas1.ID); // Binding the first texture atlas. This contains all the blocks

            glUseProgram(sManager.textureShaderProgram); // Using the program used to render textures in 3D space.

            camera.Inputs(window, deltaTime, planet); // Processing the inputs like keyboard input or mouse movement.
            camera.Matrix(45.0f, 0.1f, 500.0f, sManager.textureShaderProgram, "camMatrix"); // Calculating the camera matrix, crucial for 3d space

            planet->Update(camera, sManager.texmmLoc); // Updating the planet, this includes removing and adding blocks, rendering chunks if crossed
            planet->Render(); // Rendering the entire world

            glfwPollEvents(); // Polling for events like inputs
            glfwSwapBuffers(window); // Swapping the back and front buffers
        }
    }
    void Terminate()
    {
        glfwTerminate(); // Destroy the window
    }

private:
    GLFWwindow* window;
    Camera camera = Camera::Camera(800, 800, glm::vec3(0.0f, 60.0f, 1.0f));
    Planet* planet;
    Crosshair crosshair;
    Hotbar hotbar;
    TextureManager tManager;
    ShaderManager sManager;
    double deltaTime, prevTime;
};
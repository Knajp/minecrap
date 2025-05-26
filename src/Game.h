#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "HUD.h"
#include "Graphics.h"
#include <json/yyjson.h>
#include "Menu.h"
#include "Text.h"
#include "Logger.h"
#include <string>
#include <sstream>
//  Callback for handling GLFW errors
void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
//  Callback for window resizes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
std::string SAVEFILE = "balls"; // test
std::string filePath = "saves/" + std::string(SAVEFILE) + ".json";
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
        glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 3); // Implementation of mild antialiasing

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        camera.Init(mode->width, mode->height, glm::vec3(0.0f, 60.0f, 1.0f));
        window = glfwCreateWindow(mode->width, mode->height, "Minecrap", NULL, NULL); // Create a window 800x800

        float aspect = (float)mode->width / (float)mode->height;
        
        glfwMakeContextCurrent(window); //  Critical: creating an OpenGL context
        
        int64_t seed = time(NULL); 

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window);
            glfwTerminate(); // Exit if glad (basically OpenGL) failed to load
        }

        

        glEnable(GL_MULTISAMPLE); //    Enabling antialiasing
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //   Enabling window resize

        
        glViewport(0, 0, mode->width, mode->height); // The viewport will be 800x800px

        glClearColor(0.639f, 0.8f, 0.984f, 1.0f); // Sky colour
        
        
        glEnable(GL_TEXTURE_2D);
        glUseProgram(sManager.invertedShaderProgram);

        tManager = TextureManager(); // Creating the texture manager
        tManager.Init(); // This funcion exists to prevent throws
        sManager = ShaderManager(1); //Creating the shaderManager
        tRend = TextRenderer(aspect);
        //Menu Part
        Menu mainMenu(tManager.titleScreen.ID, tManager.bgDirt.ID, aspect);
        
        SAVEFILE = mainMenu.Loop(sManager.invertedShaderProgram, sManager.textShaderProgram, window, tManager.menuButton, tManager.wmanager, tManager.wsave, mode, &tRend);
        filePath = "saves/" + std::string(SAVEFILE) + ".json";

        std::cout << filePath << "\n";

        std::fstream exists(filePath);
        if (exists.is_open())
        {
            std::cout << "file open\n";
            std::ifstream inFile(filePath);
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            std::string contents = buffer.str();

            const char* json = contents.c_str();

            yyjson_doc* doc = yyjson_read(json, strlen(json), 0);
            yyjson_val* root = yyjson_doc_get_root(doc);
            yyjson_val* gameObj = yyjson_arr_get(root, 0);

            yyjson_val* savedSeed = yyjson_obj_get(gameObj, "seed");
            seed = (int64_t)yyjson_get_num(savedSeed);

            inFile.close();
        }
        else
        {
            std::cout << "file created\n";
            std::ofstream File(filePath);
            File << "[\n\n]";
            File.close();

            std::ifstream inFile(filePath);
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            std::string contents = buffer.str();

            const char* json = contents.c_str();

            yyjson_doc* doc = yyjson_read(json, strlen(json), 0);

            yyjson_mut_doc* mut_doc = yyjson_doc_mut_copy(doc, NULL);
            yyjson_mut_val* mut_root = yyjson_mut_doc_get_root(mut_doc);

            yyjson_mut_val* gameObj = yyjson_mut_obj(mut_doc);
            yyjson_mut_obj_add_int(mut_doc, gameObj, "seed", seed);
            yyjson_mut_obj_add_str(mut_doc, gameObj, "save", "def");

            yyjson_mut_arr_append(mut_root, gameObj);

            const char* out_json = yyjson_mut_write(mut_doc, YYJSON_WRITE_PRETTY, NULL);

            std::ofstream outFile(filePath);
            outFile << out_json;
            outFile.close();
            inFile.close();
        }
        

        glUseProgram(sManager.invertedShaderProgram);

        GLint transLoc = glGetUniformLocation(sManager.invertedShaderProgram, "xTrans");
        glUniform1f(transLoc, 0);

        GLint yLoc = glGetUniformLocation(sManager.invertedShaderProgram, "yTrans");
        glUniform1f(yLoc, 0);

        
        glDisable(GL_CULL_FACE); // Transparency stuff
        glEnable(GL_BLEND); // More Transparency stuff
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Even more transparency stuff
        glEnable(GL_DEPTH_TEST); // Enabling depth tests for no weird visual errors

        
        logger = Logger(&tRend);
        logger.createLog(DEBUG);
        logger.createLog(NO_PASSX);
        logger.createLog(NO_PASSZ);
        logger.createLog(XINCHUNK);
        logger.createLog(ZINCHUNK);
        logger.createLog(CHUNKX);
        logger.createLog(CHUNKZ);
        logger.createLog(OPENGL_ERROR);
        logger.createLog(FALLING);
        logger.createLog(BLOCKBELOW);

        crosshair = Crosshair::Crosshair(1, aspect); // Creating the crosshair using a dummy constructor
        hotbar = Hotbar::Hotbar(1, aspect); // Creating the hotbar using a dummy constructor
        planet = new Planet(5, sManager.texmmLoc, seed); //   Critical: creating the planet

        


        deltaTime = 0.0f; // This will be used for calculating things like movement per second. Every single unit dependant on time will now be per second.
        prevTime = glfwGetTime(); // Current time

	}
    void GameLoop()
    {
        logger.updateLog<std::string>(DEBUG, "bitter lemon sucks");
        logger.updateLog<std::string>(NO_PASSX, "Free");
        logger.updateLog<std::string>(NO_PASSZ, "Free");
        logger.updateLog<short int>(XINCHUNK, 0);
        logger.updateLog<short int>(ZINCHUNK, 0);
        logger.updateLog<int>(CHUNKX, 0);
        logger.updateLog<int>(CHUNKZ, 0);
        logger.updateLog<std::string>(OPENGL_ERROR, "NONE");
        logger.updateLog<uint16_t>(BLOCKBELOW, 0);
        logger.updateLog<std::string>(FALLING, "FALSE");
        
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

            camera.Inputs(window, deltaTime, planet, &logger); // Processing the inputs like keyboard input or mouse movement.
            camera.Matrix(90.0f, 0.1f, 500.0f, sManager.textureShaderProgram, "camMatrix"); // Calculating the camera matrix, crucial for 3d spac

            planet->Update(camera, sManager.texmmLoc); // Updating the planet, this includes removing and adding blocks, rendering chunks if crossed
            planet->Render(); // Rendering the entire world

            logger.Display(sManager.textShaderProgram);
            glfwPollEvents(); // Polling for events like inputs
            glfwSwapBuffers(window); // Swapping the back and front buffers
        }
    }
    void Terminate()
    {
        planet->Save();
        glfwTerminate(); // Destroy the window
    }

private:
    GLFWwindow* window;
    Camera camera;
    Planet* planet;
    Crosshair crosshair;
    Hotbar hotbar;
    TextureManager tManager;
    ShaderManager sManager;
    TextRenderer tRend;
    Logger logger;
    double deltaTime, prevTime;
};
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "Camera.h"
#include "structs.h"
#include "World.h"
#include "Texture.h"
#include "HUD.h"
/*
    TODO:
        - fix white lines pls
*/
#pragma region formalities
void glfw_error_callback(int error, const char* description) {
	std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLFWwindow* init()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		std::cerr << "Failed to initialize GLFW";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 3);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Minecrap", NULL, NULL);

	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

    glEnable(GL_MULTISAMPLE);
	return window;
}
#pragma endregion
	
const char* vertexShaderSource = "#version 430 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 col;\n"
"uniform mat4 camMatrix;\n"
"uniform mat4 modelMatrix;\n"
"void main()\n"
"{\n"
"   gl_Position = camMatrix * modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	col = aColor;"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 col;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(col, 1.0f);\n"
"}\0";

const char* tVertexShaderSource =
"#version 430 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"uniform mat4 camMatrix;\n"
"uniform mat4 modelMatrix;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"    gl_Position = camMatrix * modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"    ourColor = aColor;\n"
"    TexCoord = aTexCoord;\n"
"}\0";

const char* tFragmentShaderSource =
"#version 430 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
    "vec4 texColor = texture(ourTexture, TexCoord);\n"
    "if(texColor.a == 0)\n"
    "   discard;\n"
"    FragColor = texColor;\n"
"}\0";

const char* invertedVertexShaderSource =
"#version 430 core\n"
"layout(location = 0) in vec2 inPos;\n"
"layout(location = 1) in vec2 texCoord;\n"
"uniform mat4 proj;"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * vec4(inPos, 0.0, 1.0);"
"   TexCoord = inTexCoords;"
"{\0";

const char* invertedFragmentShaderSource =
"#version 430 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   vec4 texColor = texture(ourTexture, TexCoord);\n"
"   if(ourTexture.a == 0)\n"
"       discard;\n"
"   texColor.a = 0.8;\n"
"   FragColor = texColor;\n"
"}\0";


int main(int argc, char** argv) {
    GLFWwindow* window = init();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    GLuint tVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(tVertexShader, 1, &tVertexShaderSource, NULL);
    glCompileShader(tVertexShader);

    GLuint tFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(tFragShader, 1, &tFragmentShaderSource, NULL);
    glCompileShader(tFragShader);

    GLuint textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, tVertexShader);
    glAttachShader(textureShaderProgram, tFragShader);
    glLinkProgram(textureShaderProgram);

    glDeleteShader(tVertexShader);
    glDeleteShader(tFragShader);

    GLuint iVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(iVertexShader, 1, &invertedVertexShaderSource, NULL);
    glCompileShader(iVertexShader);

    GLuint iFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(iFragmentShader, 1, &invertedFragmentShaderSource, NULL);
    glCompileShader(iFragmentShader);

    GLuint invertedShaderProgram = glCreateProgram();
    glAttachShader(invertedShaderProgram, iVertexShader);
    glAttachShader(invertedShaderProgram, iFragmentShader);
    glLinkProgram(invertedShaderProgram);

    glDeleteShader(iVertexShader);
    glDeleteShader(iFragmentShader);
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    Camera camera(800, 800, glm::vec3(0.0f, 68.0f, 1.0f));

    glViewport(0, 0, 800, 800);

    const TextureAtlas atlas1 = TextureAtlas("src/texture/ta1.png");
    const Texture crosshairTexture = Texture("src/texture/crosshair.png");

    Crosshair crosshair;

    glUseProgram(shaderProgram);
    GLuint mmLoc = glGetUniformLocation(shaderProgram, "modelMatrix");
    GLuint texmmLoc = glGetUniformLocation(textureShaderProgram, "modelMatrix");

    Planet* planet = new Planet(5, texmmLoc, time(NULL));

    glClearColor(0.639f, 0.8f, 0.984f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    double deltaTime = 0.0f;
    double prevTime = glfwGetTime();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
        deltaTime = currentTime - prevTime;
        prevTime = currentTime;




        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas1.ID);

        glUseProgram(textureShaderProgram);

        

        camera.Inputs(window, deltaTime, planet);
        camera.Matrix(45.0f, 0.1f, 500.0f, textureShaderProgram, "camMatrix");

        planet->Update(camera, texmmLoc);
        planet->Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}

#include "Game.h"
/*
    Credits Damian Kêpczyk 2025
    ---------------------------
    Minecraft clone made using OpenGL (glad) and GLFW.
    External dependencies:
        - OpenSimplexNoise
        - GLM
        - premake (for project files)
        - STB
    ---------------------------
    Started January 2025
    Finished ?
*/
const char* textVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 tCoord;
        out vec2 TexCoords;

        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(aPos, 0.0, 1.0);
            TexCoords = tCoord;
        }  
    )";

const char* textFragmentShaderSource = R"(
    #version 330 core
    in vec2 tCoord;
    out vec4 color;

    uniform sampler2D text;
    uniform vec3 textColor;

    void main()
    {    
        float alpha = texture(text, tCoord).r;
        color = vec4(textColor, alpha); 
    } 
    )";
// O CHUJ TU KURWA CHODZI
void TestTextRendering() {
    if (!glfwInit())
        std::cerr << "Failed to initialize GLFW"; // Notify of GLFW failure
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //   Window hints (Version x., Version .x, profile)
    glfwWindowHint(GLFW_SAMPLES, 3); // Implementation of mild antialiasing

    GLFWwindow* window = glfwCreateWindow(800, 600, "Text Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Initialize TextRenderer with fixed aspect ratio (800x600)
    TextRenderer textRenderer(800.0f / 600.0f);

    // Simplified shader (use your existing shaders)
    GLuint textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(textVertexShader, 1, &textVertexShaderSource, NULL);
    glCompileShader(textVertexShader);


    GLuint textFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(textFragShader, 1, &textFragmentShaderSource, NULL);
    glCompileShader(textFragShader);


    GLuint textShaderProgram = glCreateProgram();
    glAttachShader(textShaderProgram, textVertexShader);
    glAttachShader(textShaderProgram, textFragShader);
    glLinkProgram(textShaderProgram);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.639f, 0.8f, 0.984f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        textRenderer.RenderText(textShaderProgram, "A E", -1.0f, -1.0f, 0.1f, {0.0f, 1.0f, 0.0f});
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
int main(int argc, char** argv) 
{
    TestTextRendering();
    Game game; // Create the game object
    game.GameLoop(); // Processing the entire game
    game.Terminate(); // Turn the game off
    return 0;
}

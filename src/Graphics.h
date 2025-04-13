#pragma once
#include <glad/glad.h>
#include <GLFW/GLFW3.h>



class ShaderManager
{
public:
    ShaderManager()
    {

    }
    ShaderManager(int d)
    {
        GLuint tVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(tVertexShader, 1, &tVertexShaderSource, NULL);
        glCompileShader(tVertexShader);

        GLuint tFragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(tFragShader, 1, &tFragmentShaderSource, NULL);
        glCompileShader(tFragShader);

        textureShaderProgram = glCreateProgram();
        glAttachShader(textureShaderProgram, tVertexShader);
        glAttachShader(textureShaderProgram, tFragShader);
        glLinkProgram(textureShaderProgram);

        glDeleteShader(tVertexShader);
        glDeleteShader(tFragShader);

        GLuint iVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(iVertexShader, 1, &HUDvertexShaderSource, NULL);
        glCompileShader(iVertexShader);

        GLuint iFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(iFragmentShader, 1, &HUDfragmentShaderSource, NULL);
        glCompileShader(iFragmentShader);

        invertedShaderProgram = glCreateProgram();
        glAttachShader(invertedShaderProgram, iVertexShader);
        glAttachShader(invertedShaderProgram, iFragmentShader);
        glLinkProgram(invertedShaderProgram);

        texmmLoc = glGetUniformLocation(textureShaderProgram, "modelMatrix");
    }
    GLuint invertedShaderProgram, textureShaderProgram;
    GLuint texmmLoc;

    bool checkShaderCompileErrors(GLuint shaderID) {
        GLint success;
        GLchar infoLog[1024];

        // Check for compile errors
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
            std::cerr << "Shader Compilation Failed: " << infoLog << std::endl;
            return false;
        }

        return true;
    }
private:
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

    const char* HUDvertexShaderSource = R"(
    #version 430 core
    layout(location = 0) in vec2 inPos;
    layout(location = 1) in vec2 texCoord;

    uniform mat4 projection;
    uniform float xTrans;

    out vec2 TexCoord;

    void main() {
        vec2 transPos = inPos + vec2(xTrans, 0.0f);
        gl_Position = projection * vec4(transPos, 0.0, 1.0);
        TexCoord = texCoord;
    })";

    const char* HUDfragmentShaderSource = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform sampler2D ourTexture;

    void main() {
        vec4 texColor = texture(ourTexture, TexCoord);
        if (texColor.a < 0.1f) {
            discard;
        }
        FragColor = texColor;
    })";
};


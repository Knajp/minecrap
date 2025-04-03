#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "World.h"

class Camera
{
public:
    glm::vec3 m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_movementOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int m_Width, m_Height;

    float m_Speed = 2.0f;
    float m_Sens = 100.0f;
    unsigned short int selectedBlock = 0;
    BLOCKTYPE inventory[9] = { PINKWOOL, MAGEWOOL, REDWOOL, PURPWOOL, YELLWOOL, BLUEWOOL, CYANWOOL, PLANKS, WOOD };

private:
    glm::vec3 m_Position;
    bool spacePress = false;
    bool MB1Press = false;
    bool MB2Press = false;
    bool inAir = false;
public:
    Camera(int width, int height, glm::vec3 position)
        : m_Width(width), m_Height(height), m_Position(position) {}

    glm::vec3 getPosition() const
    {
        return m_Position;
    }
    void Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderProgram, const char* uniform)
    {
        glm::mat4 view = glm::mat4(1.0f), projection = glm::mat4(1.0f);
        view = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
        projection = glm::perspective(glm::radians(FOVdeg), (float)(m_Width / m_Height), nearPlane, farPlane);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
    }

    /*
        TODO:
            - add gravity
    */
    void Gravity(Planet* planet, double deltaTime)
    {
        int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
        int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE));
        int xinchunk = int(m_Position.x) - ChunkX * CHUNK_SIZE;
        int zinchunk = int(m_Position.z) - ChunkZ * CHUNK_SIZE;

        ChunkData* data = planet->getChunkData({ ChunkX, ChunkZ });


        int blockbelow = 0;

        for (int i = 255; i >= 0; i--)
        {
            if(i < this->m_Position.y - 1.75 && i > blockbelow && data->GetBlock(xinchunk, i, zinchunk) != AIR && std::find(billboards.begin(), billboards.end(), data->GetBlock(xinchunk, i, zinchunk)) == billboards.end())
                blockbelow = i + 1;

        }
        if (data->GetBlock(xinchunk, int(m_Position.y), zinchunk) != AIR)
        {
            m_Velocity.y *= 0.7f;
            m_Velocity.y = -m_Velocity.y;
            m_Position.y = int(m_Position.y) - 0.1f;

        }
        if (m_Position.y - 1.75 > blockbelow)
        {
           m_Velocity.y -= 9.81f * float(deltaTime);
           inAir = true;

        }
        else // If is not in the air
        {
            if (inAir) // if in air is true
            {
                m_Velocity.y = 0.0f; // set vertical velocity to 0
                m_Position.y = blockbelow + 1.75f;
                inAir = false; // set inAir to false
            }
        }
            

        
    }
    void Inputs(GLFWwindow* window, double deltaTime, Planet* planet)
    {
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_Velocity.x += m_Speed * m_movementOrientation.x; // x is Y????
            m_Velocity.z += m_Speed * m_movementOrientation.z;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_Velocity.x += m_Speed * -glm::normalize(glm::cross(m_movementOrientation, m_Up)).x;
            m_Velocity.z += m_Speed * -glm::normalize(glm::cross(m_movementOrientation, m_Up)).z;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_Velocity.x += m_Speed * -m_movementOrientation.x;
            m_Velocity.z += m_Speed * -m_movementOrientation.z;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_Velocity.x += m_Speed * glm::normalize(glm::cross(m_movementOrientation, m_Up)).x;
            m_Velocity.z += m_Speed * glm::normalize(glm::cross(m_movementOrientation, m_Up)).z;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            m_Position.y = 60.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (!spacePress && !inAir)
                m_Velocity.y += 4.5f;
            spacePress = 1;
        }
        else spacePress = 0;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            m_Speed = 0.4f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        {
            m_Speed = 0.2f;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            selectedBlock = 0;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            selectedBlock = 1;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            selectedBlock = 2;
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            selectedBlock = 3;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            selectedBlock = 4;
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            selectedBlock = 5;
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            selectedBlock = 6;
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            selectedBlock = 7;
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            selectedBlock = 8;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !MB1Press)
        {
            
            int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
            int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE));
            for (float i = 1.1f; i <= 5.0f; i += 0.1f)
            {
                glm::vec3 point = m_Position + i * glm::normalize(m_Orientation);

                glm::ivec3 intVec(
                    static_cast<int>(glm::floor(point.x)) - ChunkX * CHUNK_SIZE,
                    static_cast<int>(glm::floor(point.y)),
                    static_cast<int>(glm::floor(point.z)) - ChunkZ * CHUNK_SIZE
                );

                if (planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) != AIR)
                {
                    std::cout << intVec.x << " " << intVec.y << " " << intVec.z << " " << planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) << "\n";
                    planet->getChunkData({ ChunkX,ChunkZ })->UpdateBlock(intVec.x, intVec.y, intVec.z, AIR);
                    std::cout << intVec.x << " " << intVec.y << " " << intVec.z << " " << planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) << "\n";
                    break;
                }

            }
            MB1Press = true;
        }



        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            MB1Press = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !MB2Press)
        {

            int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
            int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE));
            glm::vec3 prevPoint = { 0,0,0 };

            glm::vec3 targetBlock;
            for (float i = 1.1f; i <= 5.0f; i += 0.1f)
            {
                glm::vec3 point = m_Position + i * glm::normalize(m_Orientation);

                glm::ivec3 intVec(
                    static_cast<int>(glm::floor(point.x)) - ChunkX * CHUNK_SIZE,
                    static_cast<int>(glm::floor(point.y)),
                    static_cast<int>(glm::floor(point.z)) - ChunkZ * CHUNK_SIZE
                );

                

                if (planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) != AIR)
                {
                    if ((intVec.z >= point.z && intVec.z <= prevPoint.z) || (intVec.z <= point.z && intVec.z >= prevPoint.z))
                    {
                        targetBlock = intVec;
                        targetBlock.z -= 1;

                        break;
                    }
                    else if ((intVec.z + 1 >= point.z && intVec.z + 1 <= prevPoint.z) || (intVec.z + 1 <= point.z && intVec.z + 1 >= prevPoint.z))
                    {
                        targetBlock = intVec;
                        targetBlock.z += 1;
                        break;
                    }

                    else if ((intVec.x >= point.x && intVec.x <= prevPoint.x) || (intVec.x <= point.x && intVec.x >= prevPoint.x))
                    {
                        targetBlock = intVec;
                        targetBlock.x -= 1;
                        break;
                    }
                    else if ((intVec.x + 1 >= point.x && intVec.x + 1 <= prevPoint.x) || (intVec.x + 1 <= point.x && intVec.x + 1 >= prevPoint.x))
                    {
                        targetBlock = intVec;
                        targetBlock.x += 1;
                        break;
                    }
                    else if ((intVec.y >= point.y && intVec.y <= prevPoint.y) || (intVec.y <= point.y && intVec.y >= prevPoint.y))
                    {
                        targetBlock = intVec;
                        targetBlock.y -= 1;
                        break;
                    }
                    else if ((intVec.y + 1 >= point.y && intVec.y + 1 <= prevPoint.y) || (intVec.y + 1 <= point.y && intVec.y + 1 >= prevPoint.y))
                    {
                        targetBlock = intVec;
                        targetBlock.y += 1;
                        break;
                    }
                }
                prevPoint = point;
            }

            if(targetBlock != glm::vec3(0, 0, 0))
                planet->getChunkData({ ChunkX, ChunkZ })->UpdateBlock(int(targetBlock.x), int(targetBlock.y), int(targetBlock.z), inventory[selectedBlock]);
            MB2Press = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        {
            MB2Press = false;
        }

        Gravity(planet, deltaTime);

        int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
        int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE));

        int xinchunk = int(m_Position.x) - ChunkX * CHUNK_SIZE;
        int zinchunk = int(m_Position.z) - ChunkZ * CHUNK_SIZE;
        int yinchunk = int(m_Position.y);
        
        if (xinchunk > CHUNK_SIZE) ChunkX += 1;
        if (zinchunk > CHUNK_SIZE) ChunkZ += 1;

        if (xinchunk < 0) ChunkX -= 1;
        if (zinchunk < 0) ChunkZ -= 1;
        ChunkData* chunkD = planet->getChunkData({ ChunkX, ChunkZ });
        xinchunk = int(m_Position.x) - ChunkX * CHUNK_SIZE;
        zinchunk = int(m_Position.z) - ChunkZ * CHUNK_SIZE;

        if ((chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk, zinchunk) == AIR ||
            std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk, zinchunk)) != billboards.end()) &&
            (chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk - 1, zinchunk) == AIR ||
                std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk - 1, zinchunk)) != billboards.end())) {
            m_Position.x += m_Velocity.x * float(deltaTime);
        }
        if ((chunkD->GetBlock(xinchunk, yinchunk, int(m_Position.z + m_Velocity.z * float(deltaTime))) == AIR ||
            std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(xinchunk, yinchunk, int(m_Position.z + m_Velocity.z * float(deltaTime)))) != billboards.end()) &&
            (chunkD->GetBlock(xinchunk, yinchunk - 1, int(m_Position.z + m_Velocity.z * float(deltaTime))) == AIR ||
                std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(xinchunk, yinchunk - 1, int(m_Position.z + m_Velocity.z * float(deltaTime)))) != billboards.end())) {
            m_Position.z += m_Velocity.z * float(deltaTime);
        }
       
        m_Position.y += m_Velocity.y * float(deltaTime);

        float dampingFactor = 0.96f;
        m_Velocity.x *= dampingFactor;
        m_Velocity.z *= dampingFactor;
       


            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            double x, y;
            glfwGetCursorPos(window, &x, &y);

            float rotx = m_Sens * (float)(y - 400) / 800;
            float roty = m_Sens * (float)(x - 400) / 800;

            glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(-rotx), glm::normalize(glm::cross(m_Orientation, m_Up)));

            if (!((glm::angle(newOrientation, m_Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -m_Up) <= glm::radians(5.0f))))
            {
                m_Orientation = newOrientation;
            }

            m_Orientation = glm::rotate(m_Orientation, glm::radians(-roty), m_Up);
            m_movementOrientation = glm::rotate(m_movementOrientation, glm::radians(-roty), m_Up);

            glfwSetCursorPos(window, 400, 400);

    }

};
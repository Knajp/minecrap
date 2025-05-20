#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "World.h"
#include "Logger.h"

// The camera (player) class
class Camera
{
public:
    glm::vec3 m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_movementOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int m_Width, m_Height;

    int playerHealth = 8;

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
    Camera() {}// The camera constructor, taking in the viewport width, height and initial player position{}
    void Init(int width, int height ,glm::vec3 position)
    {
        m_Width = width;
        m_Height = height;
        m_Position = position;
    }
    glm::vec3 getPosition() const // Returns current player position
    {
        return m_Position;
    }
    void Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderProgram, const char* uniform) const // Used for calculating the camera matirx 
    {
        glm::mat4 view = glm::mat4(1.0f), projection = glm::mat4(1.0f); //  Initializing the view and proj matrices as identitiy
        view = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up); //    The direction the camera is facing
        projection = glm::perspective(glm::radians(FOVdeg), (float)m_Width / (float)m_Height, nearPlane, farPlane); // Dark magic lol

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, uniform), 1, GL_FALSE, glm::value_ptr(projection * view)); // Send the matrix over to shader
    }

    void Gravity(Planet* planet, double deltaTime) // The gravity applying function
    {
        int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE)); // The X coordinate of the chunk player is in
        int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE)); // The Z coordinate of the chunk player is in
        int xinchunk = int(m_Position.x) - ChunkX * CHUNK_SIZE; // Player's x positon in chunk
        int zinchunk = int(m_Position.z) - ChunkZ * CHUNK_SIZE; // Player's z position in chunk

        ChunkData* data = planet->getChunkData({ ChunkX, ChunkZ }); // Data(blocks) of the chunks the player is in


        int blockbelow = 0; // initializing the value storing the first block below the player

        for (int i = 255; i >= 0; i--) // Looping through all the blocks in player's block column
        {
            // If the block is not higher than the player's feet and is not air nor a billboard
            if(i < this->m_Position.y - 1.75 && i > blockbelow && data->GetBlock(xinchunk, i, zinchunk) != AIR && std::find(billboards.begin(), billboards.end(), data->GetBlock(xinchunk, i, zinchunk)) == billboards.end())
                blockbelow = i + 1; // The blockbelow value will be set to the top face of the block

        }
        if (data->GetBlock(xinchunk, int(m_Position.y), zinchunk) != AIR) // If the player's head is touching a block
        {
            m_Velocity.y *= 0.7f; // Transfer 30% of energy to block
            m_Velocity.y = -m_Velocity.y; // Reverse velocity to fall down
            m_Position.y = int(m_Position.y) - 0.1f; // Prevent teleport

        }
        if (m_Position.y - 1.75 > blockbelow) // If above the ground
        {
           m_Velocity.y -= 9.81f * float(deltaTime); // Apply the gravity (9.81m/s^2)
           inAir = true; // Set the inAir value to true

        }
        else // If on the ground
        {
            if (inAir) // And it's the first contact with the ground
            {
                if (m_Velocity.y < -10.0f) // If the y velocity is greater than 10m/s
                {
                    playerHealth += int(m_Velocity.y) / 2; // Apply fall damage
                }
                m_Velocity.y = 0.0f; // Remove all velocity
                m_Position.y = blockbelow + 1.75f; // Correct players y positon
                inAir = false; // set inAir to false
            }
        }
            

        
    }
    void Inputs(GLFWwindow* window, double deltaTime, Planet* planet, Logger* logger) // The function for processing input
    {
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // If the W key is held
        {
            m_Velocity.x += m_Speed * m_movementOrientation.x; 
            m_Velocity.z += m_Speed * m_movementOrientation.z; // Add forward velocity to the player
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // If the A key is held
        {
            m_Velocity.x += m_Speed * -glm::normalize(glm::cross(m_movementOrientation, m_Up)).x;
            m_Velocity.z += m_Speed * -glm::normalize(glm::cross(m_movementOrientation, m_Up)).z; // Add leftward velocity to the player
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // If the S key is held
        {
            m_Velocity.x += m_Speed * -m_movementOrientation.x;
            m_Velocity.z += m_Speed * -m_movementOrientation.z; // Add backward velocity to the player
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // If the D key is held
        {
            m_Velocity.x += m_Speed * glm::normalize(glm::cross(m_movementOrientation, m_Up)).x;
            m_Velocity.z += m_Speed * glm::normalize(glm::cross(m_movementOrientation, m_Up)).z; // Add rightward velocity to the player
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) // The debug key
        {
            m_Position.y = 60.0f; // Debug function
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // If Space is pressed
        {
            if (!spacePress && !inAir) // If it's the first press and the player is on the ground
                m_Velocity.y += 5.0f; // Add upwards velocity
            spacePress = true; // Negate the first contact value
        }
        else spacePress = false; // If it's not then the next one will be first press
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // If left shift is pressed
        {
            m_Speed = 0.4f; // Increase player speed
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) // If it is not
        {
            m_Speed = 0.2f; // Turn it back to normal
        }
        //  Block switiching
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
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // If escape is pressed
        {
            glfwSetWindowShouldClose(window, true); // Exit the app
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !MB1Press) // If left mouse is pressed and it's not the first contact
        {
            
            int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
            int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE)); // Chunk X and Z coordinates
            for (float i = 1.1f; i <= 5.0f; i += 0.1f) // Casting a ray
            {
                glm::vec3 point = m_Position + i * glm::normalize(m_Orientation); // Point on the ray

                glm::ivec3 intVec(
                    static_cast<int>(glm::floor(point.x)) - ChunkX * CHUNK_SIZE,
                    static_cast<int>(glm::floor(point.y)),
                    static_cast<int>(glm::floor(point.z)) - ChunkZ * CHUNK_SIZE
                ); // Block coordinates of the ray

                if (planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) != AIR) // If it is not air
                {
                    planet->getChunkData({ ChunkX,ChunkZ })->UpdateBlock(intVec.x, intVec.y, intVec.z, AIR);
                    break; // Remove the block and end the ray
                }

            }
            MB1Press = true; // It is not first contact
        }



        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) // If left mouse is released
        {
            MB1Press = false; // It is first contact
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !MB2Press) // If right mouse is pressed and it is first contact
        {

            int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
            int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE)); // Chunk X and Z coordinates
            glm::vec3 prevPoint = { 0,0,0 }; // Previous point on the ray

            glm::vec3 targetBlock; // Target block coordinates
            for (float i = 1.1f; i <= 5.0f; i += 0.1f) // Cast a ray
            {
                glm::vec3 point = m_Position + i * glm::normalize(m_Orientation); // Current point on the ray

                glm::ivec3 intVec(
                    static_cast<int>(glm::floor(point.x)) - ChunkX * CHUNK_SIZE,
                    static_cast<int>(glm::floor(point.y)),
                    static_cast<int>(glm::floor(point.z)) - ChunkZ * CHUNK_SIZE
                ); // Block coordinates of the ray

                

                if (planet->getChunkData({ ChunkX,ChunkZ })->GetBlock(intVec.x, intVec.y, intVec.z) != AIR)
                {
                    if ((intVec.z >= point.z && intVec.z <= prevPoint.z) || (intVec.z <= point.z && intVec.z >= prevPoint.z)) // If the south face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.z -= 1;
                        // the target will be in the south
                        break;
                    }
                    else if ((intVec.z + 1 >= point.z && intVec.z + 1 <= prevPoint.z) || (intVec.z + 1 <= point.z && intVec.z + 1 >= prevPoint.z)) // If the north face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.z += 1;
                        // The target will be in the north
                        break;
                    }

                    else if ((intVec.x >= point.x && intVec.x <= prevPoint.x) || (intVec.x <= point.x && intVec.x >= prevPoint.x)) // If the east face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.x -= 1;
                        //The target will be in the east
                        break;
                    }
                    else if ((intVec.x + 1 >= point.x && intVec.x + 1 <= prevPoint.x) || (intVec.x + 1 <= point.x && intVec.x + 1 >= prevPoint.x)) // If the west face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.x += 1;
                        // The target will be in the west
                        break;
                    }
                    else if ((intVec.y >= point.y && intVec.y <= prevPoint.y) || (intVec.y <= point.y && intVec.y >= prevPoint.y)) // If the bottom face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.y -= 1;
                        // The target will be on the bottom
                        break;
                    }
                    else if ((intVec.y + 1 >= point.y && intVec.y + 1 <= prevPoint.y) || (intVec.y + 1 <= point.y && intVec.y + 1 >= prevPoint.y)) // If the top face is between or on the points
                    {
                        targetBlock = intVec;
                        targetBlock.y += 1;
                        // The target will be on the top
                        break;
                    }
                }
                prevPoint = point; // The previous point will be set to the current one
            }

            if(targetBlock != glm::vec3(0, 0, 0)) // Prevent throws
                planet->getChunkData({ ChunkX, ChunkZ })->UpdateBlock(int(targetBlock.x), int(targetBlock.y), int(targetBlock.z), inventory[selectedBlock]); // Set the target block to selected block
            
            MB2Press = true; // Not first contact anymore
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) // If right mouse is released
        {
            MB2Press = false; // it is first contact
        }

        Gravity(planet, deltaTime); // Apply gravity

        int ChunkX = static_cast<int>(floor(m_Position.x / CHUNK_SIZE));
        int ChunkZ = static_cast<int>(floor(m_Position.z / CHUNK_SIZE)); // Chunk X and Z coordinates

        int xinchunk = int(m_Position.x) - ChunkX * CHUNK_SIZE;
        int zinchunk = int(m_Position.z) - ChunkZ * CHUNK_SIZE; // Player's x and z in the chunk
        int yinchunk = int(m_Position.y); // Player's y
        
        ChunkData* chunkD = planet->getChunkData({ ChunkX, ChunkZ });


        //Prevent going into blocks
        if ((chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk, zinchunk) == AIR ||
            std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk, zinchunk)) != billboards.end()) &&
            (chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk - 1, zinchunk) == AIR ||
                std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(int(m_Position.x + m_Velocity.x * float(deltaTime)), yinchunk - 1, zinchunk)) != billboards.end())) {
            m_Position.x += m_Velocity.x * float(deltaTime);
            logger->updateLog<std::string>(NO_PASS, "Free");
        }
        else logger->updateLog<std::string>(NO_PASS, "BLOCKED");
        //Prevent going into blocks
        if ((chunkD->GetBlock(xinchunk, yinchunk, int(m_Position.z + m_Velocity.z * float(deltaTime))) == AIR ||
            std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(xinchunk, yinchunk, int(m_Position.z + m_Velocity.z * float(deltaTime)))) != billboards.end()) &&
            (chunkD->GetBlock(xinchunk, yinchunk - 1, int(m_Position.z + m_Velocity.z * float(deltaTime))) == AIR ||
                std::find(billboards.begin(), billboards.end(), chunkD->GetBlock(xinchunk, yinchunk - 1, int(m_Position.z + m_Velocity.z * float(deltaTime)))) != billboards.end())) {
            m_Position.z += m_Velocity.z * float(deltaTime);
            logger->updateLog<std::string>(NO_PASS, "Free");

        }
        else logger->updateLog<std::string>(NO_PASS, "BLOCKED");
        m_Position.y += m_Velocity.y * float(deltaTime); // Apply velocity to position in the y direction

        float dampingFactor = 0.96f;
        m_Velocity.x *= dampingFactor;
        m_Velocity.z *= dampingFactor; // Damp x and z velocity
       


            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide cursor

            double x, y;
            glfwGetCursorPos(window, &x, &y); // Get the cursor position

            float rotx = m_Sens * (float)(y - 400) / 800;
            float roty = m_Sens * (float)(x - 400) / 800; // Rotation of mouse in x and y

            glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(-rotx), glm::normalize(glm::cross(m_Orientation, m_Up))); // Calculate the new orientation

            if (!((glm::angle(newOrientation, m_Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -m_Up) <= glm::radians(5.0f)))) // Prevent camera from doing a barrel roll
            {
                m_Orientation = newOrientation;
            }

            m_Orientation = glm::rotate(m_Orientation, glm::radians(-roty), m_Up); // Always able to rotate around
            m_movementOrientation = glm::rotate(m_movementOrientation, glm::radians(-roty), m_Up); // The orientation for movement

            glfwSetCursorPos(window, 400, 400); // Centre cursor

    }

};
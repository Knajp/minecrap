#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "structs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "OpenSimplexNoise.h"

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <fstream>
#include <sstream>
#include <json/yyjson.h>
#include "Texture.h"

extern std::string filePath; //test

class Camera;
/*
    TODO: 
        - find unoptimized fragments and optimize
*/
constexpr unsigned short int  CHUNK_SIZE = 16;
constexpr unsigned short int CHUNK_HEIGHT = 128;


enum BLOCKTYPE
{
    AIR, GRASS, DIRT, STONE, TALLGRASS, ROSE, BLUEFLOWER, WOOD, LEAVES,
    PURPWOOL, PINKWOOL, REDWOOL, BLUEWOOL, YELLWOOL, CYANWOOL, GRAYWOOL, MAGEWOOL,
    PLANKS, IRONORE, DIAMONDORE, GOLDORE, PINKORE
};
enum FACE
{
    NORTH, SOUTH, EAST, WEST, TOP, BOTTOM
};

const std::vector<BLOCKTYPE> fullBlocks = { GRASS, DIRT, STONE, WOOD, LEAVES, PURPWOOL, PINKWOOL, REDWOOL, BLUEWOOL, YELLWOOL, CYANWOOL, GRAYWOOL, MAGEWOOL, PLANKS };
const std::vector<BLOCKTYPE> billboards = { TALLGRASS, ROSE, BLUEFLOWER };
const std::vector<BLOCKTYPE> transparentBlocks = { LEAVES };

struct ChunkData
{
    ChunkData(uint16_t* data)
        : chunkData(data)
    {

    }
    ~ChunkData()
    {
        delete[] chunkData;
    }

    uint16_t* chunkData;
    bool updateRequired = false;
    std::vector<std::pair<int, uint16_t>> changed;

    static int GetIndex(int x, int y, int z)
    {
        return x * CHUNK_HEIGHT * CHUNK_SIZE + z * CHUNK_HEIGHT + y;
    }

    uint16_t GetBlock(int x, int y, int z) const
    {
        if (x > 15 || x < 0 || y > 256 || y < 0 || z > 16 || z < 0) return AIR;
        return chunkData[GetIndex(x, y, z)];
    }
    
    void UpdateBlock(int x, int y, int z, BLOCKTYPE type)
    {
        if (GetIndex(x, y, z) > CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE || GetIndex(x, y, z) < 0) return;
        chunkData[GetIndex(x, y, z)] = type;
        changed.push_back({GetIndex(x,y,z), type});
        updateRequired = true;
    }
};

struct ChunkPosHash
{
    std::size_t operator()(const glm::vec2& key) const
    {
        std::size_t hx = std::hash<int>()(static_cast<int>(key.x)); // Hash x
        std::size_t hy = std::hash<int>()(static_cast<int>(key.y)); // Hash y

        // Combine the hashes using bitwise XOR and shifting
        return hx ^ (hy << 1); // No need for a z component
    }
};

class Chunk
{
public:
    Chunk(glm::vec2 _worldPos, GLuint modelLoc, uint16_t* data)
        :   m_WorldPos(_worldPos), modelMatLoc(modelLoc), modelMatrix(1.0f), m_data(data)
    {
        GenChunk();
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO); 
        glGenBuffers(1, &EBO);

        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, m_totalVertices.size() * sizeof(textureVertex), m_totalVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_totalIndices.size() * sizeof(GLushort), m_totalIndices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)(3*sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)(6 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &bVAO);
        glBindVertexArray(bVAO);

        glGenBuffers(1, &bVBO);
        glGenBuffers(1, &bEBO);


        glBindBuffer(GL_ARRAY_BUFFER, bVBO);
        glBufferData(GL_ARRAY_BUFFER, m_billboardVertices.size() * sizeof(textureVertex), m_billboardVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices.size() * sizeof(GLushort), m_billboardIndices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)(6 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_WorldPos.x * CHUNK_SIZE, 0.0f, m_WorldPos.y * CHUNK_SIZE));
    }
    void Reload()
    {
        // Clear old data before regenerating the chunk
        m_totalVertices.clear();
        m_totalIndices.clear();
        m_billboardVertices.clear();
        m_billboardIndices.clear();

        // Regenerate the chunk's data
        GenChunk();

        // After generating new chunk data, update the buffer data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, m_totalVertices.size() * sizeof(textureVertex), m_totalVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_totalIndices.size() * sizeof(GLushort), m_totalIndices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, bVBO);
        glBufferData(GL_ARRAY_BUFFER, m_billboardVertices.size() * sizeof(textureVertex), m_billboardVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices.size() * sizeof(GLushort), m_billboardIndices.data(), GL_STATIC_DRAW);

        m_data.updateRequired = false;
    }
    void SaveToJSON()
    {
        std::ifstream inFile(filePath);

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string contents = buffer.str();

        const char* json = contents.c_str();

        yyjson_doc* doc = yyjson_read(json, strlen(json), 0);
        if (!doc) {
            std::cout << "JSON parse failed\n";
            return;
        }
        yyjson_val* root = yyjson_doc_get_root(doc);

        yyjson_mut_doc* mut_doc = yyjson_doc_mut_copy(doc, NULL);
        yyjson_mut_val* mut_root = yyjson_mut_doc_get_root(mut_doc);

        yyjson_mut_val* newObj = yyjson_mut_obj(mut_doc);
        yyjson_mut_obj_add_int(mut_doc, newObj, "locX", (int64_t)m_WorldPos.x);
        yyjson_mut_obj_add_int(mut_doc, newObj, "locZ", (int64_t)m_WorldPos.y);

        yyjson_mut_val* dataObj = yyjson_mut_obj(mut_doc);
        yyjson_mut_val* dataKey = yyjson_mut_str(mut_doc, "data");
        for (const auto& pair : m_data.changed)
        {
            std::string strValue = std::to_string(pair.first);
            yyjson_mut_val* key = yyjson_mut_strcpy(mut_doc, strValue.c_str());
            yyjson_mut_val* val = yyjson_mut_int(mut_doc, pair.second);
            yyjson_mut_obj_add(dataObj, key, val);
        }
        yyjson_mut_obj_add(newObj, dataKey, dataObj);

        yyjson_mut_arr_append(mut_root, newObj);

        const char* out_json = yyjson_mut_write(mut_doc, YYJSON_WRITE_PRETTY, NULL);

        std::ofstream outFile(filePath);
        outFile << out_json;
        outFile.close();

        yyjson_doc_free(doc);
        yyjson_mut_doc_free(mut_doc);
        free((void*)out_json);
    }
    void Render() const
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL uniformmatrix4fv error: " << err << std::endl;
        }

        glDrawElements(GL_TRIANGLES, GLsizei(m_totalIndices.size()), GL_UNSIGNED_SHORT, NULL);

        glBindVertexArray(bVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bEBO);

        glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL uniformmatrix4fv error: " << err << std::endl;
        }

        glDrawElements(GL_TRIANGLES, GLsizei(m_billboardIndices.size()), GL_UNSIGNED_SHORT, NULL);

        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL draw elements error: " << err << std::endl;
        }
    }
    ChunkData* getData()
    {
        return &m_data;
    }
    glm::vec2 getWorldPosition() const
    {
        return m_WorldPos;
    }
private:
    void GenChunk()
    {
        unsigned int indexOffset = 0;
        unsigned int bilOffset = 0;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    uint16_t currentBlock = m_data.GetBlock(x, y, z);
                    if (currentBlock == AIR || currentBlock == 0) {
                        continue;
                    }
                    
                    else if (std::find(billboards.begin(), billboards.end(), currentBlock) != billboards.end())
                    {
                        {
                            Vector2f texCoord = getBlockTexture(currentBlock, NULL);
                            m_billboardVertices.push_back({ float(x + 0.1), float(y), float(z + 0.1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                            m_billboardVertices.push_back({ float(x + 0.1), float(y + 0.8), float(z + 0.1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });
                            m_billboardVertices.push_back({ float(x + 0.9), float(y), float(z + 0.9), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y});
                            m_billboardVertices.push_back({ float(x + 0.9), float(y + 0.8), float(z + 0.9), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize});

                            m_billboardIndices.push_back(bilOffset + 0);
                            m_billboardIndices.push_back(bilOffset + 1);
                            m_billboardIndices.push_back(bilOffset + 2);
                            m_billboardIndices.push_back(bilOffset + 2);
                            m_billboardIndices.push_back(bilOffset + 3);
                            m_billboardIndices.push_back(bilOffset + 1);

                            bilOffset += 4;
                        }

                        {
                            Vector2f texCoord = getBlockTexture(currentBlock, NULL);
                            m_billboardVertices.push_back({ float(x + 0.9), float(y), float(z + 0.1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                            m_billboardVertices.push_back({ float(x + 0.9), float(y + 0.8), float(z + 0.1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize});
                            m_billboardVertices.push_back({ float(x + 0.1), float(y), float(z + 0.9), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                            m_billboardVertices.push_back({ float(x + 0.1), float(y + 0.8), float(z + 0.9), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });

                            m_billboardIndices.push_back(bilOffset + 0);
                            m_billboardIndices.push_back(bilOffset + 1);
                            m_billboardIndices.push_back(bilOffset + 2);
                            m_billboardIndices.push_back(bilOffset + 2);
                            m_billboardIndices.push_back(bilOffset + 3);
                            m_billboardIndices.push_back(bilOffset + 1);

                            bilOffset += 4;
                        }
                    }
                    else // Full Blocks
                    {
                        //  North - positive Z
                        {
                            if (z == CHUNK_SIZE - 1 || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x, y, z + 1)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x, y, z + 1)) != transparentBlocks.end() || m_data.GetBlock(x, y, z + 1) == AIR) {
                                Vector2f texCoord = getBlockTexture(currentBlock, NORTH);
                                m_totalVertices.push_back({ float(x), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }

                        //  South - negative Z
                        {
                            if (z == 0 || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x, y, z - 1)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x, y, z - 1)) != transparentBlocks.end() || m_data.GetBlock(x, y, z - 1) == AIR) {
                                Vector2f texCoord = getBlockTexture(currentBlock, SOUTH);
                                m_totalVertices.push_back({ float(x), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }

                        //  East - positive X
                        {
                            if (x == CHUNK_SIZE - 1 || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x + 1, y, z)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x + 1, y, z)) != transparentBlocks.end() || m_data.GetBlock(x + 1, y, z) == AIR)
                            {
                                Vector2f texCoord = getBlockTexture(currentBlock, EAST);
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }

                        // West - negative X
                        {
                            if (x == 0 || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x - 1, y, z)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x - 1, y, z)) != transparentBlocks.end() || m_data.GetBlock(x - 1, y, z) == AIR) {
                                Vector2f texCoord = getBlockTexture(currentBlock, WEST);
                                m_totalVertices.push_back({ float(x), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }

                        // Top - positive Y
                        {
                            if (y == CHUNK_SIZE - 1 || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x, y + 1, z)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x, y + 1, z)) != transparentBlocks.end() || m_data.GetBlock(x, y + 1, z) == AIR) {
                                Vector2f texCoord = getBlockTexture(currentBlock, TOP);
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x + 1), float(y + 1), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }

                        // Bottom - negative Y
                        {
                            if (y == 0 || m_data.GetBlock(x, y - 1, z) == AIR || std::find(billboards.begin(), billboards.end(), m_data.GetBlock(x, y - 1, z)) != billboards.end() || std::find(transparentBlocks.begin(), transparentBlocks.end(), m_data.GetBlock(x, y - 1, z)) != transparentBlocks.end() || m_data.GetBlock(x, y - 1, z) == AIR) { // always generate bottom
                                Vector2f texCoord = getBlockTexture(currentBlock, BOTTOM);
                                m_totalVertices.push_back({ float(x), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y });
                                m_totalVertices.push_back({ float(x), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y });
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z), 0.0f, 0.0f, 0.0f, texCoord.x + tileSize, texCoord.y + tileSize });
                                m_totalVertices.push_back({ float(x + 1), float(y), float(z + 1), 0.0f, 0.0f, 0.0f, texCoord.x, texCoord.y + tileSize });

                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 3);
                                m_totalIndices.push_back(indexOffset + 1);
                                m_totalIndices.push_back(indexOffset + 0);
                                m_totalIndices.push_back(indexOffset + 2);
                                m_totalIndices.push_back(indexOffset + 3);

                                indexOffset += 4;
                            }
                        }
                    }
                }
            }
        }
    }
    glm::vec2 m_WorldPos;


    std::vector<textureVertex> m_totalVertices;
    std::vector<GLushort> m_totalIndices;

    std::vector<textureVertex> m_billboardVertices;
    std::vector<GLushort> m_billboardIndices;

    

    GLuint VAO, VBO, EBO;
    GLuint bVAO, bVBO, bEBO;

    ChunkData m_data;

    glm::mat4 modelMatrix;
    GLuint modelMatLoc;
};

class Planet
{
public:
    Planet(int size, GLuint modelLoc, int64_t seed)
        :noiseGenerator(seed), noise2D(seed)
    {
        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {

                uint16_t* chunkD = LoadChunkAtPos(x, z);
                // Position chunks in world space using the offset calculated
                glm::vec2 chunkPosition(x, z);
                Chunk* chunk = new Chunk(chunkPosition, modelLoc, chunkD);

                // Save chunk and chunk data with the correct world position
                chunks[chunk->getWorldPosition()] = chunk;
                chunkData[chunk->getWorldPosition()] = chunk->getData();
            }
        }
    }
    uint16_t* LoadChunkAtPos(int x, int z)
    {
        std::vector<std::pair<int, int>> treePositions;

        uint16_t* chunkD = new uint16_t[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
        int size = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
        std::vector<int> skipIndices;

        for (int ax = 0; ax < CHUNK_SIZE; ax++) {
            for (int az = 0; az < CHUNK_SIZE; az++) {
                double noiseValue = noiseGenerator.eval((x * CHUNK_SIZE + ax) * 0.05, (z * CHUNK_SIZE + az) * 0.05);
                double noise2dValue = noise2D.eval((x * CHUNK_SIZE + ax) * 0.3, (z * CHUNK_SIZE + az) * 0.3);

                int height = static_cast<int>((noiseValue + 1.0) * 3 + 50);  // Adjust height scaling
                int surface = static_cast<int>((noise2dValue + 1.0) * 20);

                for (int ay = 0; ay < CHUNK_HEIGHT; ay++) {
                    int index = ChunkData::GetIndex(ax, ay, az);
                    if (std::find(skipIndices.begin(), skipIndices.end(), index) != skipIndices.end()) continue;

                    if (ay <= height) {
                        if (ay < height - 1) {
                            chunkD[index] = STONE;
                        }
                        else if (ay == height - 1) {
                            chunkD[index] = DIRT;
                        }
                        else {
                            chunkD[index] = GRASS;
                        }
                    }
                    else if (ay == height + 1 && surface < 16)
                    {
                        chunkD[index] = TALLGRASS;
                    }
                    else if (ay == height + 1 && surface == 16)
                    {
                        int flower = rand() % 2 + 1;
                        switch (flower)
                        {
                        case 1:
                            chunkD[index] = ROSE;
                            break;
                        case 2:
                            chunkD[index] = BLUEFLOWER;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (ay == height + 1 && surface == 18)
                    {
                        bool canPlaceTree = true;

                        for (const auto& tree : treePositions) {
                            int treeX = tree.first;
                            int treeZ = tree.second;

                            if (std::abs(ax - treeX) < 5 && std::abs(az - treeZ) < 5) {
                                canPlaceTree = false;
                                break;
                            }
                        }

                        if (canPlaceTree)
                        {
                            treePositions.push_back({ ax, az });
                            for (int i = 0; i <= 4; i++)
                            {
                                chunkD[ChunkData::GetIndex(ax, ay + i, az)] = WOOD;
                                skipIndices.push_back(ChunkData::GetIndex(ax, ay + i, az));
                                if (chunkD[ChunkData::GetIndex(ax, ay + i, az)] == 52685) chunkD[ChunkData::GetIndex(ax, ay + i, az)] = AIR;

                            }
                            for (int i = -2; i <= 2; i++)
                            {
                                for (int j = -2; j <= 2; j++)
                                {
                                    if (i == 0 && j == 0) continue;
                                    if (ChunkData::GetIndex(ax + i, ay + 2, az + j) < size && ChunkData::GetIndex(ax + i, ay + 2, az + j) > 0)
                                    {

                                        chunkD[ChunkData::GetIndex(ax + i, ay + 2, az + j)] = LEAVES;
                                        skipIndices.push_back(ChunkData::GetIndex(ax + i, ay + 2, az + j));
                                        if(chunkD[ChunkData::GetIndex(ax + i, ay + 2, az + j)] = 52685) chunkD[ChunkData::GetIndex(ax + i, ay + 2, az + j)] = AIR;
                                    }
                                }
                            }
                            for (int i = -2; i <= 2; i++)
                            {
                                for (int j = -2; j <= 2; j++)
                                {
                                    if (i == 0 && j == 0) continue;
                                    if (i == -2 && i == j) continue;
                                    if (i == 2 && i == j) continue;
                                    if (i == -2 && abs(i) == j) continue;
                                    if (i == 2 && i == abs(j)) continue;
                                    if (ChunkData::GetIndex(ax + i, ay + 3, az + j) < size && ChunkData::GetIndex(ax + i, ay + 3, az + j) > 0)
                                    {

                                        chunkD[ChunkData::GetIndex(ax + i, ay + 3, az + j)] = LEAVES;
                                        skipIndices.push_back(ChunkData::GetIndex(ax + i, ay + 3, az + j));
                                        if(chunkD[ChunkData::GetIndex(ax + i, ay + 3, az + j)] == 52685) chunkD[ChunkData::GetIndex(ax + i, ay + 3, az + j)] = AIR;
                                    }
                                }
                            }
                            for (int i = 4; i <= 5; i++)
                            {
                                if (ChunkData::GetIndex(ax - 1, ay + i, az) < size && ChunkData::GetIndex(ax - 1, ay + i, az) > 0)
                                {
                                    chunkD[ChunkData::GetIndex(ax - 1, ay + i, az)] = LEAVES;
                                    skipIndices.push_back(ChunkData::GetIndex(ax - 1, ay + i, az));
                                }
                                if (ChunkData::GetIndex(ax + 1, ay + i, az) < size && ChunkData::GetIndex(ax + 1, ay + i, az) > 0)
                                {
                                    chunkD[ChunkData::GetIndex(ax + 1, ay + i, az)] = LEAVES;
                                    skipIndices.push_back(ChunkData::GetIndex(ax + 1, ay + i, az));
                                }
                                if (ChunkData::GetIndex(ax, ay + i, az - 1) < size && ChunkData::GetIndex(ax, ay + i, az - 1) > 0)
                                {
                                    chunkD[ChunkData::GetIndex(ax, ay + i, az - 1)] = LEAVES;
                                    skipIndices.push_back(ChunkData::GetIndex(ax, ay + i, az - 1));
                                }
                                if (ChunkData::GetIndex(ax, ay + i, az + 1) < size && ChunkData::GetIndex(ax, ay + i, az + 1) > 0)
                                {
                                    chunkD[ChunkData::GetIndex(ax, ay + i, az + 1)] = LEAVES;
                                    skipIndices.push_back(ChunkData::GetIndex(ax, ay + i, az + 1));
                                }
                            }
                            chunkD[ChunkData::GetIndex(ax, ay + 5, az)] = LEAVES;
                            chunkD[ChunkData::GetIndex(ax, ay + 6, az)] = LEAVES;
                            skipIndices.push_back(ChunkData::GetIndex(ax, ay + 5, az));
                            skipIndices.push_back(ChunkData::GetIndex(ax, ay + 6, az));
                        }
                        if (chunkD[index] == 52685) chunkD[index] = AIR;
                    }
                    else {
                        chunkD[index] = AIR; // Air above the terrain
                    }
                }
            }
        }
        

        std::ifstream inFile(filePath);
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string contents = buffer.str();
        const char* json = contents.c_str();

        yyjson_doc* doc = yyjson_read(json, contents.size(), 0);
        yyjson_val* root = yyjson_doc_get_root(doc);

        size_t idx, max;
        yyjson_val* val;

        yyjson_arr_foreach(root, idx, max, val) {
            if (idx == 0) continue;
            yyjson_val* X = yyjson_obj_get(val, "locX");
            yyjson_val* Z = yyjson_obj_get(val, "locZ");

            int64_t chunkX = (int64_t)yyjson_get_num(X);
            int64_t chunkZ = (int64_t)yyjson_get_num(Z);
            if (chunkX == x && chunkZ == z)
            {
                yyjson_val* data = yyjson_obj_get(val, "data");
                yyjson_val* key;
                yyjson_val* val2;
                size_t idx2, max2;
                yyjson_obj_foreach(data, idx2, max2, key, val2)
                {
                    const char* keyStr = yyjson_get_str(key);
                    if (!keyStr) continue;
                    int64_t index = std::stoll(keyStr);
                    uint16_t block = (uint16_t)yyjson_get_num(val2);
                    chunkD[index] = block;
                }
            }
        }
        return chunkD;
    }
    ~Planet()
    {
        for (auto& entry : chunks)
        {
            delete entry.second;
        }
    }
    void Render() const
    {
        for (const auto& chunkPair : chunks)
        {
            const Chunk* chunk = chunkPair.second;
            chunk->Render();
        }
    }

    void Update(const Camera& camera, GLuint modelLoc);
    
    ChunkData* getChunkData(const glm::vec2& pos) const
    {
        auto it = chunkData.find(pos);
        if (it != chunkData.end()) {
            return it->second; 
        }
        return nullptr;  
    }
    void Save()
    {
        for (const auto& chunk : chunks)
        {
            chunk.second->SaveToJSON();
        }
    }
public:
    static Planet* planet;

private:
    glm::vec3 lastPlrPosition;
    glm::ivec2 lastPlrChunk;

    int renderDistance = 5;

    OpenSimplexNoise::Noise noise2D;
    OpenSimplexNoise::Noise noiseGenerator;

    std::unordered_map<glm::vec2, Chunk*, ChunkPosHash> chunks;
    std::unordered_map<glm::vec2, ChunkData*, ChunkPosHash> chunkData;
};

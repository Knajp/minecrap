#include "World.h"
#include "Camera.h"

void Planet::Update(const Camera& camera, GLuint modelLoc)
{
    glm::vec3 currentPlrPosition = camera.getPosition();

    glm::ivec2 currentPlrChunk;
    currentPlrChunk.x = static_cast<int>(floor(currentPlrPosition.x / CHUNK_SIZE));
    currentPlrChunk.y = static_cast<int>(floor(currentPlrPosition.z / CHUNK_SIZE));

    if (currentPlrChunk != lastPlrChunk)
    {
        std::unordered_map<glm::vec2, Chunk*, ChunkPosHash> nchunks;
        std::unordered_map<glm::vec2, ChunkData*, ChunkPosHash> nchunkData;

        for (int x = -((renderDistance - 1) / 2); x <= ((renderDistance - 1) / 2); x++)
        {
            for (int z = -((renderDistance - 1) / 2); z <= ((renderDistance - 1) / 2); z++)
            {
                glm::vec2 worldPos = { currentPlrChunk.x + x, currentPlrChunk.y + z };

                auto it = chunks.find(worldPos);
                if (it != chunks.end())
                {
                    nchunks[worldPos] = it->second;
                    nchunkData[worldPos] = chunkData[worldPos];
                }
                else
                {
                    uint16_t* chunkD = LoadChunkAtPos(int(worldPos.x), int(worldPos.y));
                    Chunk* chunk = new Chunk(worldPos, modelLoc, chunkD);
                    nchunks[worldPos] = chunk;
                    nchunkData[worldPos] = chunk->getData();
                }
            }
        }

        for (auto it = chunks.begin(); it != chunks.end();)
        {
            if (nchunks.find(it->first) == nchunks.end())
            {
                delete it->second;
                it = chunks.erase(it);
            }
            else
            {
                ++it;
            }
        }

        chunks = std::move(nchunks);
        chunkData = std::move(nchunkData);
    }
    for (const auto& pair : chunks)
    {
        if (pair.second->getData()->updateRequired)
        {
            pair.second->Reload();
        }
    }

    lastPlrPosition = currentPlrPosition;
    lastPlrChunk = currentPlrChunk;
}

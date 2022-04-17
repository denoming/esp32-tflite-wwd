#include "MemoryPool.hpp"

#include <esp_assert.h>

MemoryPool::MemoryPool()
{
    for (int i = 0; i < ChunkCount; ++i) {
        _chunks[i] = new Chunk;
    }
}

MemoryPool::~MemoryPool()
{
    for (int i = 0; i < ChunkCount; ++i) {
        delete _chunks[i];
    }
}

std::int16_t* MemoryPool::get(std::size_t index)
{
    assert(index < ChunkCount);
    return _chunks[index]->payload();
}


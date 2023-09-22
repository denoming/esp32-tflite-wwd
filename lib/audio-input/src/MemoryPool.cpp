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

void MemoryPool::set(size_t index, uint16_t value)
{
    assert(index < capacity());
    const long chunkIdx = (index / ChunkSize) % ChunkCount;
    const long chunkPos = (index % ChunkSize);
    _chunks[chunkIdx]->set(chunkPos, value);
}

uint16_t MemoryPool::get(size_t index) const
{
    assert(index < capacity());
    const long chunkIdx = (index / ChunkSize) % ChunkCount;
    const long chunkPos = (index % ChunkSize);
    return _chunks[chunkIdx]->get(chunkPos);
}


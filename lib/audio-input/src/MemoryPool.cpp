#include "MemoryPool.hpp"

#include <esp_assert.h>

MemoryPool::MemoryPool()
    : _chunks{}
{
    allocate();
}

MemoryPool::~MemoryPool()
{
    deallocate();
}

void
MemoryPool::set(size_t index, int16_t value)
{
    assert(index < capacity());
    const long chunkIdx = long(index / ChunkSize) % long(ChunkCount);
    const long chunkPos = long(index % ChunkSize);
    _chunks[chunkIdx]->set(chunkPos, value);
}

int16_t
MemoryPool::get(size_t index) const
{
    assert(index < capacity());
    const long chunkIdx = long(index / ChunkSize) % long(ChunkCount);
    const long chunkPos = long(index % ChunkSize);
    return _chunks[chunkIdx]->get(chunkPos);
}

void
MemoryPool::allocate()
{
    for (int i = 0; i < ChunkCount; ++i) {
        _chunks[i] = new Chunk;
    }
}

void
MemoryPool::deallocate()
{
    for (int i = 0; i < ChunkCount; ++i) {
        delete _chunks[i];
        _chunks[i] = nullptr;
    }
}

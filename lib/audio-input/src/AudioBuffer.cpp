#include "AudioBuffer.hpp"

#include <esp_assert.h>

#include "MemoryPool.hpp"

AudioBuffer::AudioBuffer(MemoryPool* memoryPool)
    : _memoryPool{memoryPool}
    , _chunk{nullptr}
    , _chunkPos{0}
    , _chunkIdx{0}
{
    assert(memoryPool != nullptr);
    _chunk = memoryPool->get(0);
}

void AudioBuffer::set(std::int16_t sample)
{
    assert(_chunk != nullptr);
    assert(_chunkPos < MemoryPool::ChunkSize);
    _chunk[_chunkPos] = sample;
}

std::int16_t AudioBuffer::peek() const
{
    assert(_chunk != nullptr);
    assert(_chunkPos < MemoryPool::ChunkSize);
    return _chunk[_chunkPos];
}

bool AudioBuffer::next()
{
    _chunkPos++;
    if (_chunkPos >= MemoryPool::ChunkSize) {
        _chunkPos = 0;
        _chunkIdx = (_chunkIdx + 1) % MemoryPool::ChunkCount;
        _chunk = _memoryPool->get(_chunkIdx);
        return true;
    }
    return false;
}

bool AudioBuffer::put(std::int16_t sample)
{
    set(sample);
    return next();
}

int AudioBuffer::pos() const
{
    return (_chunkIdx * MemoryPool::ChunkSize + _chunkPos);
}

void AudioBuffer::seek(int index)
{
    int totalSize = MemoryPool::capacity();
    index  = (index + totalSize) % totalSize;

    _chunkIdx = (index / MemoryPool::ChunkSize) % MemoryPool::ChunkCount;
    _chunkPos = (index % MemoryPool::ChunkSize);

    assert(_chunkIdx < MemoryPool::ChunkCount);
    _chunk = _memoryPool->get(_chunkIdx);
}

AudioBuffer AudioBuffer::clone()
{
    AudioBuffer buffer{_memoryPool};
    buffer.seek(pos());
    return buffer;
}
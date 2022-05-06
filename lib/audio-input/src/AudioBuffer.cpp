#include "AudioBuffer.hpp"

#include <esp_assert.h>

#include "MemoryPool.hpp"

AudioBuffer::AudioBuffer(MemoryPool& memoryPool)
    : _memoryPool{memoryPool}
    , _index{0}
{
}

void AudioBuffer::set(std::int16_t sample)
{
    assert(_index < MemoryPool::capacity());
    _memoryPool.set(_index, sample);
}

std::int16_t AudioBuffer::peek() const
{
    assert(_index < MemoryPool::capacity());
    return _memoryPool.get(_index);
}

void AudioBuffer::next()
{
    _index = (_index + 1) % MemoryPool::capacity();
}

void AudioBuffer::put(std::int16_t sample)
{
    set(sample);
    next();
}

int AudioBuffer::pos() const
{
    return _index;
}

void AudioBuffer::seek(int index)
{
    int totalSize = MemoryPool::capacity();
    _index  = (index + totalSize) % totalSize;
}

AudioBuffer AudioBuffer::clone()
{
    AudioBuffer buffer{_memoryPool};
    buffer.seek(pos());
    return buffer;
}
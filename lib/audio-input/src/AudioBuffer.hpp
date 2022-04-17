#pragma once

#include <memory>
#include <vector>

class MemoryPool;

class AudioBuffer {
public:
    explicit AudioBuffer(MemoryPool* memoryPool);

    void set(std::int16_t sample);

    std::int16_t get(std::size_t index) const;

    std::int16_t peek() const;

    bool next();

    bool put(std::int16_t sample);

    int pos() const;

    void seek(int index);

    AudioBuffer clone();

private:
    MemoryPool* _memoryPool;
    std::int16_t* _chunk;
    std::size_t _chunkPos;
    std::size_t _chunkIdx;
};
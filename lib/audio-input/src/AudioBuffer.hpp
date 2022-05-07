#pragma once

#include <memory>
#include <vector>

class MemoryPool;

class AudioBuffer final {
public:
    explicit AudioBuffer(MemoryPool& memoryPool);

    void set(std::int16_t sample);

    std::int16_t get() const;

    void put(std::int16_t sample);

    std::int16_t next();

    int pos() const;

    void seek(int index);

    AudioBuffer clone();

private:
    MemoryPool& _memoryPool;
    std::size_t _index;
};
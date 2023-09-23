#pragma once

#include <memory>
#include <vector>

class MemoryPool;

class AudioBuffer final {
public:
    explicit AudioBuffer(MemoryPool& memoryPool);

    void
    set(std::int16_t sample);

    [[nodiscard]] int16_t
    get() const;

    void
    put(int16_t sample);

    [[nodiscard]] int16_t
    next();

    [[nodiscard]] int
    pos() const;

    void
    seek(int index);

    [[nodiscard]] AudioBuffer
    clone();

private:
    MemoryPool& _memoryPool;
    size_t _index;
};
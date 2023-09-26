#pragma once

#include <memory>
#include <vector>

class MemoryPool;

class AudioBuffer final {
public:
    explicit AudioBuffer(MemoryPool& memoryPool);

    void
    set(int16_t sample);

    [[nodiscard]] int16_t
    get() const;

    void
    put(int16_t sample);

    [[maybe_unused]] int16_t
    next();

    [[nodiscard]] size_t
    pos() const;

    void
    seek(int index);

    [[nodiscard]] AudioBuffer
    clone();

private:
    MemoryPool& _memoryPool;
    size_t _index;
};
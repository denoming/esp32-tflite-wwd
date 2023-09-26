#pragma once

#include <array>

#include <esp_types.h>

class MemoryPool final {
public:
    MemoryPool();

    ~MemoryPool();

    static inline size_t
    capacity();

    void
    set(size_t index, int16_t value);

    [[nodiscard]] int16_t
    get(size_t index) const;

private:
    static const size_t ChunkCount = 5;
    static const size_t ChunkSize = 16000;

    class Chunk {
    public:
        Chunk()
            : _payload{}
        {
        }

        void
        set(size_t index, int16_t value)
        {
            _payload[index] = value;
        }

        [[nodiscard]] int16_t
        get(size_t index) const
        {
            return _payload[index];
        }

    private:
        std::array<int16_t, ChunkSize> _payload;
    };

private:
    void
    allocate();

    void
    deallocate();

private:
    std::array<Chunk*, ChunkCount> _chunks;
};

// Inlines

size_t
MemoryPool::capacity()
{
    return (ChunkCount * ChunkSize);
}
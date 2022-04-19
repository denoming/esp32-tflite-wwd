#pragma once

#include <array>

class MemoryPool {
public:
    static const std::size_t ChunkCount = 10;
    static const std::size_t ChunkSize = 1600;

    class Chunk {
    public:
        Chunk()
        {
            _payload.fill(0);
        }

        std::int16_t* payload()
        {
            return _payload.data();
        }

    private:
        std::array<std::int16_t, ChunkSize> _payload;
    };

public:
    MemoryPool();

    ~MemoryPool();

    std::int16_t* get(std::size_t index);

    static inline std::size_t capacity();

private:
    std::array<Chunk*, ChunkCount> _chunks;
};

// Inlines

std::size_t MemoryPool::capacity()
{
    return (ChunkCount * ChunkSize);
}
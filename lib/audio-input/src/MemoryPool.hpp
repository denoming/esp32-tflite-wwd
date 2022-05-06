#pragma once

#include <array>

class MemoryPool final {
public:
    static const std::size_t ChunkCount = 10;
    static const std::size_t ChunkSize = 1600;

    class Chunk {
    public:
        Chunk()
        {
            _payload.fill(0);
        }

        void set(std::size_t index, std::uint16_t value)
        {
            _payload[index] = value;
        }

        std::uint16_t get(std::size_t index) const
        {
            return _payload[index];
        }

    private:
        std::array<std::int16_t, ChunkSize> _payload;
    };

public:
    MemoryPool();

    ~MemoryPool();

    static inline std::size_t capacity();

    void set(std::size_t index, std::uint16_t value);

    std::uint16_t get(std::size_t index) const;

private:
    std::array<Chunk*, ChunkCount> _chunks;
};

// Inlines

std::size_t MemoryPool::capacity()
{
    return (ChunkCount * ChunkSize);
}
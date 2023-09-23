#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/i2s_std.h>

#include "AudioBuffer.hpp"
#include "AudioInputSampler.hpp"

class MemsMicrophone final : public AudioInputSampler {
public:
    explicit MemsMicrophone(MemoryPool& memoryPool);

    bool
    start(TaskHandle_t waiter) override;

    AudioBuffer
    buffer();

private:
    size_t
    pullAudioData(uint8_t* buffer, size_t size);

    void
    processAudioData(const uint8_t* buffer, size_t size);

    void
    notify();

    [[noreturn]] static void
    pullAudioDataTask(void* param);

private:
    i2s_chan_handle_t _channelHandle;
    AudioBuffer _buffer;
    TaskHandle_t _waiter;
};

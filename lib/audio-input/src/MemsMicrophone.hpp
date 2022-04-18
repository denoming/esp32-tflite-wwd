#pragma once

#include <driver/i2s.h>

#include "AudioBuffer.hpp"
#include "AudioInputSampler.hpp"

class MemsMicrophone : public AudioInputSampler {
public:
    MemsMicrophone(i2s_pin_config_t pins,
                   i2s_port_t port,
                   i2s_config_t config,
                   MemoryPool& memoryPool);

    bool start(TaskHandle_t waiter) override;

    AudioBuffer buffer();

private:
    void pullData();

    void processData(const uint8_t* data, std::size_t size);

    static void pullDataTask(void* param);

private:
    i2s_pin_config_t _pins;
    i2s_port_t _port;
    i2s_config_t _config;
    AudioBuffer _buffer;
    QueueHandle_t _queue;    
    TaskHandle_t _waiter;
};


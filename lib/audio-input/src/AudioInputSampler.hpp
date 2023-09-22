#pragma once

#include <freertos/task.h>

class AudioInputSampler {
public:
    virtual ~AudioInputSampler() = default;

    virtual bool
    start(TaskHandle_t waiter)
        = 0;
};
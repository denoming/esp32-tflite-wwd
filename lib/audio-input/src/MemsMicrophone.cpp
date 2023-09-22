#include "MemsMicrophone.hpp"

#include <esp_log.h>
#include <esp_assert.h>
#include <freertos/task.h>

#include "Config.hpp"

static const char* TAG = "ESP32 TFLITE WWD - MemsMicrophone";

MemsMicrophone::MemsMicrophone(i2s_pin_config_t pins,
                               i2s_port_t port,
                               i2s_config_t config,
                               MemoryPool& memoryPool)
    : _pins{pins}
    , _port{port}
    , _config{config}
    , _buffer{memoryPool}
    , _waiter{nullptr}
{
}

bool
MemsMicrophone::start(TaskHandle_t waiter)
{
    static const int kQueueSize = 8;
    static const uint32_t kTaskStackDepth = 4096u;
    static const UBaseType_t kTaskPriority
        = UBaseType_t((tskIDLE_PRIORITY + 1) | portPRIVILEGE_BIT);

    if (i2s_driver_install(_port, &_config, kQueueSize, &_queue) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver");
        return false;
    }

    if (i2s_set_pin(_port, &_pins) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pin");
        return false;
    }

    _waiter = waiter;
    const auto rv = xTaskCreate(&MemsMicrophone::pullDataTask,
                                "MEMS microphone pull data task",
                                kTaskStackDepth,
                                this,
                                kTaskPriority,
                                nullptr);
    if (rv != pdPASS) {
        _waiter = nullptr;
        ESP_LOGE(TAG, "Failed to create pull data task");
        return false;
    }

    return true;
}

AudioBuffer
MemsMicrophone::buffer()
{
    return _buffer.clone();
}

size_t
MemsMicrophone::pullData(uint8_t* buffer, size_t size)
{
    static const TickType_t kTimeout = 100 / portTICK_PERIOD_MS;

    size_t bytesRead{0};
    i2s_event_t event;
    if (xQueueReceive(_queue, &event, portMAX_DELAY) == pdPASS) {
        if (event.type == I2S_EVENT_RX_DONE) {
            ESP_ERROR_CHECK(i2s_read(_port, buffer, size, &bytesRead, kTimeout));
        }
    }
    return bytesRead;
}

void
MemsMicrophone::processData(const uint8_t* buffer, size_t size)
{
    static const int kDataBitShift = 11;

    const auto* samples = reinterpret_cast<const int32_t*>(buffer);
    assert(samples != nullptr);
    for (int i = 0; i < size / sizeof(int32_t); ++i) {
        _buffer.put(samples[i] >> kDataBitShift);
    }
}

void
MemsMicrophone::pullDataTask(void* param)
{
    static const size_t kNotifyThreshold = 1600;
    static const size_t kBufferSize = I2S_DMA_BUFFER_LEN * I2S_SAMPLE_BYTES * I2S_SAMPLE_BYTES;

    assert(param != nullptr);
    MemsMicrophone* mic = static_cast<MemsMicrophone*>(param);

    size_t totalBytes{0};
    uint8_t buffer[kBufferSize];
    while (true) {
        size_t bytesRead{0};
        bytesRead = mic->pullData(buffer, kBufferSize);
        if (bytesRead > 0) {
            mic->processData(buffer, bytesRead);
            totalBytes += bytesRead;
            if (totalBytes >= kNotifyThreshold) {
                totalBytes -= kNotifyThreshold;
                mic->notify();
            }
        }
    }
}

void
MemsMicrophone::notify()
{
    xTaskNotify(_waiter, 1, eSetBits);
}
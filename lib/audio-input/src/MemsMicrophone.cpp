#include "MemsMicrophone.hpp"

#include <esp_log.h>
#include <esp_assert.h>
#include <freertos/task.h>

#include "Config.hpp"
#include "AudioBuffer.hpp"

static const char* TAG = "ESP32 TFLITE WWD - MemsMicrophone";

MemsMicrophone::MemsMicrophone(i2s_pin_config_t pins,
                               i2s_port_t port,
                               i2s_config_t config,
                               MemoryPool* memoryPool)
    : _pins{pins}
    , _port{port}
    , _config{config}
    , _buffer{memoryPool}
    , _waiter{nullptr}
{
}

bool MemsMicrophone::start(TaskHandle_t waiter)
{
    static const int QueueSize = 4;
    static const uint32_t TaskStackDepth = 4096u;
    static const UBaseType_t TaskPriority = UBaseType_t(1 | portPRIVILEGE_BIT);

    if (i2s_driver_install(_port, &_config, QueueSize, &_queue) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver");
        return false;
    }

    if (i2s_set_pin(_port, &_pins) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pin");
        return false;
    }

    _waiter = waiter;
    const auto rv = xTaskCreate(&MemsMicrophone::pullDataTask, "MEMS microphone pull data task",
        TaskStackDepth,
        this, TaskPriority,
        nullptr);
    if (rv != pdPASS) {
        _waiter = nullptr;
        ESP_LOGE(TAG, "Failed to create pull data task");
        return false;
    }

    return true;
}

AudioBuffer MemsMicrophone::buffer()
{
    return _buffer.clone();
}

void MemsMicrophone::pullData()
{
    static const TickType_t timeout = 10 / portTICK_PERIOD_MS;

    i2s_event_t event;
    if (xQueueReceive(_queue, &event, portMAX_DELAY) == pdPASS) {
        if (event.type == I2S_EVENT_RX_DONE) {
            std::size_t bytesRead = 0;
            do {
                uint8_t buffer[1024];
                ESP_ERROR_CHECK(i2s_read(_port, buffer, 1024, &bytesRead, 10));
                if (bytesRead > 0) {
                    processData(buffer, bytesRead);
                }
            } while (bytesRead > 0);
        }
    }
}

void MemsMicrophone::processData(const uint8_t* data, std::size_t size)
{
    assert(data != nullptr && size > 0);
    const int32_t* samples = reinterpret_cast<const int32_t*>(data);
    for (int i = 0; i < (size / 4); ++i) {
        if (_buffer.put(samples[i] >> 11)) {
            xTaskNotify(_waiter, 1, eSetBits);
        }
    }
}

void MemsMicrophone::pullDataTask(void* param)
{
    assert(param != nullptr);
    MemsMicrophone* mic = static_cast<MemsMicrophone*>(param);
    while(true) {
        mic->pullData();
    }
}
#include "MemsMicrophone.hpp"

#include <esp_log.h>
#include <esp_assert.h>

#include "Config.hpp"

static const char* TAG = "ESP32 TFLITE WWD - MemsMicrophone";

MemsMicrophone::MemsMicrophone(MemoryPool& memoryPool)
    : _channelHandle{}
    , _buffer{memoryPool}
    , _waiter{}
{
}

bool
MemsMicrophone::start(TaskHandle_t waiter)
{
    i2s_chan_config_t cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    if (i2s_new_channel(&cfg, nullptr, &_channelHandle) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to crease audio channel");
        return false;
    }

    if (i2s_channel_init_std_mode(_channelHandle, &I2S_CONFIG) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to initialize audio channel");
        return false;
    }

    if (i2s_channel_enable(_channelHandle) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to enable audio channel");
        return false;
    }

    static const uint32_t kTaskStackDepth = 4096u;
    static const BaseType_t kTaskPinnedCore = 1;

    _waiter = waiter;
    const auto rv = xTaskCreatePinnedToCore(&MemsMicrophone::pullAudioDataTask,
                                            "PULL_DATA",
                                            kTaskStackDepth,
                                            this,
                                            tskIDLE_PRIORITY,
                                            nullptr,
                                            kTaskPinnedCore);
    if (rv != pdPASS) {
        _waiter = nullptr;
        ESP_LOGE(TAG, "Unable to create pull data task");
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
MemsMicrophone::pullAudioData(uint8_t* buffer, size_t size)
{
    size_t bytesRead{0};
    ESP_ERROR_CHECK(i2s_channel_read(_channelHandle, buffer, size, &bytesRead, portMAX_DELAY));
    return bytesRead;
}

void
MemsMicrophone::processAudioData(const uint8_t* buffer, size_t size)
{
    static const int kDataBitShift = 11;

    const auto* samples = reinterpret_cast<const int32_t*>(buffer);
    assert(samples != nullptr);
    for (int i = 0; i < size / sizeof(int32_t); ++i) {
        _buffer.put(static_cast<int16_t>(samples[i] >> kDataBitShift));
    }
}

[[noreturn]] void
MemsMicrophone::pullAudioDataTask(void* param)
{
    static const size_t kNotifyThreshold = 1600;
    static const size_t kBufferSize = I2S_DMA_BUFFER_LEN * I2S_SAMPLE_BYTES * I2S_SAMPLE_BYTES;

    assert(param != nullptr);
    auto* mic = static_cast<MemsMicrophone*>(param);

    /**
     * Pulling audio data loop (PULL_DATA task: CPU1)
     */
    size_t totalBytes{0};
    uint8_t buffer[kBufferSize];
    while (true) {
        size_t bytesRead = mic->pullAudioData(buffer, kBufferSize);
        if (bytesRead > 0) {
            mic->processAudioData(buffer, bytesRead);
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
    /* send a direct to task notification  */
    xTaskNotify(_waiter, 1, eSetBits);
}
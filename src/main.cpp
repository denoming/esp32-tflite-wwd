#include <freertos/FreeRTOS.h>

#include <esp_log.h>

#include "Config.hpp"
#include "NeuralNetwork.hpp"
#include "AudioProcessor.hpp"
#include "MemsMicrophone.hpp"
#include "MemoryPool.hpp"

static const char* TAG = "ESP32 TFLITE WWD - Main";

extern "C" void
app_main()
{
    static const TickType_t kMaxBlockTime = pdMS_TO_TICKS(100);
    static const float kDetectionThreshold = 0.9;

    NeuralNetwork nn;
    if (!nn.setUp()) {
        ESP_LOGE(TAG, "Failed to set-up neural network");
        vTaskSuspend(NULL);
    }

    MemoryPool memoryPool;
    MemsMicrophone mic{I2S_PIN_CONFIG, I2S_INMP441_PORT, I2S_CONFIG, memoryPool};
    if (!mic.start(xTaskGetCurrentTaskHandle())) {
        ESP_LOGE(TAG, "Failed to start microphone");
        vTaskSuspend(NULL);
    }

    AudioProcessor audioProcessor{
        WWD_AUDIO_LENGTH, WWD_WINDOW_SIZE, WWD_STEP_SIZE, WWD_POOLING_SIZE};
    while (true) {
        const uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, kMaxBlockTime);
        if (notificationValue > 0) {
            auto buffer = mic.buffer();
            buffer.seek(buffer.pos() - I2S_SAMPLE_RATE);
            float* inputBuffer = nn.getInputBuffer();
            audioProcessor.getSpectrogram(buffer, inputBuffer);
            const float output = nn.predict();
            if (output > kDetectionThreshold) {
                ESP_LOGI(TAG, "Detected: %.2f", output);
            }
        }
    }
}

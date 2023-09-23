#include <freertos/FreeRTOS.h>

#include <esp_log.h>

#include "Config.hpp"
#include "NeuralNetwork.hpp"
#include "AudioProcessor.hpp"
#include "MemsMicrophone.hpp"
#include "MemoryPool.hpp"

static const char* TAG = "ESP32 TFLITE WWD - Main";

extern "C" [[noreturn]] void
app_main()
{
    static const TickType_t kMaxBlockTime = pdMS_TO_TICKS(300);
    static const float kDetectionThreshold = 0.9;

    NeuralNetwork nn;
    if (!nn.setUp()) {
        ESP_LOGE(TAG, "Unable to set-up neural network");
        vTaskSuspend(nullptr);
    }

    MemoryPool memoryPool;
    MemsMicrophone mic{memoryPool};
    if (!mic.start(xTaskGetCurrentTaskHandle())) {
        ESP_LOGE(TAG, "Unable to start microphone");
        vTaskSuspend(nullptr);
    }

    /**
     * Recognition loop (main task: CPU0)
     */
    AudioProcessor processor{WWD_AUDIO_LENGTH, WWD_WINDOW_SIZE, WWD_STEP_SIZE, WWD_POOLING_SIZE};
    while (true) {
        if (ulTaskNotifyTake(pdTRUE, kMaxBlockTime) > 0 /* notification value after reset */) {
            auto buffer = mic.buffer();
            buffer.seek(buffer.pos() - I2S_SAMPLE_RATE);
            float* inputBuffer = nn.getInputBuffer();
            processor.getSpectrogram(buffer, inputBuffer);
            const float output = nn.predict();
            if (output > kDetectionThreshold) {
                ESP_LOGI(TAG, "Detected: %.2f", output);
            }
        }
    }
}

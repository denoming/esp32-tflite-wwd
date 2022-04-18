#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

#include <esp_err.h>
#include <esp_log.h>

#include <esp_task_wdt.h>

#include "Config.hpp"
#include "NeuralNetwork.hpp"
#include "AudioProcessor.hpp"
#include "AudioBuffer.hpp"
#include "MemsMicrophone.hpp"
#include "MemoryPool.hpp"

static const char* TAG = "ESP32 TFLITE WWD - Main";

static MemoryPool* memoryPool;
static MemsMicrophone* mic;

void applicationTask(void *param)
{
    NeuralNetwork nn;
    nn.setUp();

    AudioProcessor audioProcessor{WWD_AUDIO_LENGTH, WWD_WINDOW_SIZE, WWD_STEP_SIZE, WWD_POOLING_SIZE};

    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
    while (true) {
        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        if (ulNotificationValue > 0) {
            auto buffer = mic->buffer();
            buffer.seek(buffer.pos() - WWD_WINDOW_SIZE);
            float* inputBuffer = nn.getInputBuffer();
            audioProcessor.getSpectrogram(buffer, inputBuffer);
            const float output = nn.predict();
            if (output > 0.5) {
                ESP_LOGI(TAG, "Detected: %.2f", output);
            } else {
                ESP_LOGI(TAG, "Not Detected: %.2f", output);
            }
        }
    }
}

extern "C" void app_main() {
    // make sure we don't get killed for our long running tasks
    esp_task_wdt_init(10, false);

    memoryPool = new MemoryPool;
    mic = new MemsMicrophone{I2S_PIN_CONFIG, I2S_INMP441_PORT, I2S_CONFIG, *memoryPool};

    TaskHandle_t applicationTaskHandle;
    xTaskCreate(applicationTask, "Application Task", 8192, nullptr, 1, &applicationTaskHandle);
    if (!mic->start(applicationTaskHandle)) {
        ESP_LOGE(TAG, "Failed to start microphone");
    }
}

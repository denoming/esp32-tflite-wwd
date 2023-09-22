#include <driver/i2s.h>

#define I2S_DMA_BUFFER_COUNT (4U)     // The total amount of DMA buffers count
#define I2S_DMA_BUFFER_LEN   (64U)    // The length of each DMA buffer in samples
#define I2S_SAMPLE_BYTES     (4U)     // The total aboumt of bytes per sample
#define I2S_SAMPLE_RATE      (16000U) // The total amount of samples per second

#define I2S_INMP441_PORT (I2S_NUM_0)
#define I2S_INMP441_SCK  (GPIO_NUM_26)
#define I2S_INMP441_WS   (GPIO_NUM_22)
#define I2S_INMP441_SD   (GPIO_NUM_21)

#define WWD_WINDOW_SIZE  (320)
#define WWD_STEP_SIZE    (160)
#define WWD_POOLING_SIZE (6)
#define WWD_AUDIO_LENGTH (I2S_SAMPLE_RATE)

const i2s_config_t I2S_CONFIG = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = I2S_DMA_BUFFER_COUNT,
    .dma_buf_len = I2S_DMA_BUFFER_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
    .mclk_multiple = I2S_MCLK_MULTIPLE_512,
    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
};

const i2s_pin_config_t I2S_PIN_CONFIG = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = I2S_INMP441_SCK,
    .ws_io_num = I2S_INMP441_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_INMP441_SD
};
#include <driver/i2s_std.h>

#define I2S_DMA_BUFFER_COUNT (4U) // The total amount of DMA buffers count
#define I2S_DMA_BUFFER_LEN (64U)  // The length of each DMA buffer in samples
#define I2S_SAMPLE_BYTES (4U)     // The total aboumt of bytes per sample
#define I2S_SAMPLE_RATE (16000U)  // The total amount of samples per second

#define I2S_INMP441_PORT (I2S_NUM_0)
#define I2S_INMP441_SCK (GPIO_NUM_26)
#define I2S_INMP441_WS (GPIO_NUM_22)
#define I2S_INMP441_SD (GPIO_NUM_21)

#define WWD_WINDOW_SIZE (320)
#define WWD_STEP_SIZE (160)
#define WWD_POOLING_SIZE (6)
#define WWD_AUDIO_LENGTH (I2S_SAMPLE_RATE)

/* Setting the configurations */
const i2s_std_config_t I2S_CONFIG = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
    /**
     * INMP1441:
     *  - 64 SCK cycles in each WS stereo frame (or 32 SCK cycles per data-word)
     *  - 24bit per channel
     *  - MSB first with one SCK cycle delay
     */
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
        .mclk = I2S_GPIO_UNUSED,
        .bclk = I2S_INMP441_SCK,
        .ws = I2S_INMP441_WS,
        .dout = I2S_GPIO_UNUSED,
        .din = I2S_INMP441_SD,
        .invert_flags = {
            .mclk_inv = false,
            .bclk_inv = false,
            .ws_inv = false,
        },
    },
};
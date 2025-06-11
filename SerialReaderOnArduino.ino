#include <driver/i2s.h>
// Missing: #include <esp_err.h> for ESP_ERROR_CHECK
// Missing: #include <stdlib.h> for llabs (though it's often implicitly included)
// Missing: #include <cmath> or <math.h> for abs/llabs (though often implicitly included by Arduino headers)

#define I2S_WS 14
#define I2S_SCK 15
#define I2S_SD 32

#define I2S_SAMPLE_RATE 16000
#define I2S_CHANNEL_NUM 1 // This is for your logic, but I2S config handles channels
#define I2S_BITS_PER_SAMPLE (I2S_BITS_PER_SAMPLE_16BIT)
#define I2S_BUFFER_SIZE 256// This is likely for dma_buf_len in bytes, not samples. Be careful with this.

void setup() {
  Serial.begin(115200);

  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // INMP441 often on right channel, check your wiring and datasheet
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = I2S_BUFFER_SIZE, // This is the length of EACH DMA buffer in *samples*, not total bytes or a single buffer size.
    .use_apll = false
  }; // ERROR 1: Missing semicolon here!

  // ERROR 2: You're calling i2s_driver_install directly here.
  // The ESP_ERROR_CHECK() macro was meant to wrap it for error handling.
  // If you don't use ESP_ERROR_CHECK, it's fine, but know that.
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  // ERROR 3: Space in 'pin config' - this was the error from last time.
  // Also, you're not including <esp_err.h> which was suggested for ESP_ERROR_CHECK
  i2s_pin_config_t pin_config = { // Should be 'pin_config'
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1, // Correct for RX
    .data_in_num = I2S_SD
  }; // ERROR 4: Missing semicolon here!

  // ERROR 5: 'pin_config' is undeclared because of ERROR 3.
  i2s_set_pin(I2S_NUM_0, &pin_config);
  // Missing: i2s_set_clk() call, which sets the actual I2S clock for the data transfer.
  // This is crucial for the microphone to work.
}

// Global declaration (correct for `audio_samples`)
// #define I2S_BUFFER_SIZE 256 // Example: 256 32-bit samples = 1024 bytes
int32_t audio_samples[I2S_BUFFER_SIZE]; // Correct way to declare a stack-allocated array for loop()

void loop() {
  size_t bytes_read = 0;

  // Read from I2S.
  esp_err_t err = i2s_read(I2S_NUM_0, (void*)audio_samples, sizeof(audio_samples), &bytes_read, portMAX_DELAY);

  if (err == ESP_OK && bytes_read > 0) {
    int64_t sum = 0;
    int num_samples_read = bytes_read / sizeof(int32_t); // If audio_samples is int32_t

    for (int i = 0; i < num_samples_read; i++) {
      sum += llabs(audio_samples[i]);
    }

    if (num_samples_read > 0) {
      int32_t average_amplitude = sum / num_samples_read;
      Serial.println(average_amplitude);
    } else {
      Serial.println("No samples read for amplitude calculation.");
    }
  } else {
    Serial.printf("I2S read error or no bytes read: %d (bytes_read: %d)\n", err, bytes_read);
  }

  delay(10);
}

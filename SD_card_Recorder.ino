#include <Arduino.h>
#include <driver/i2s.h>
#include <SD.h>
#include <SPI.h>

// I2S configuration for ESP32 and INMP441
#define I2S_WS 14
#define I2S_SCK 15
#define I2S_SD 32

#define SAMPLE_RATE     32000
#define SAMPLE_BITS     I2S_BITS_PER_SAMPLE_16BIT
#define CHANNEL_FORMAT  I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_PORT        I2S_NUM_0
#define RECORD_SECONDS  60

// SD card SPI pins
#define SD_CS    5
#define SD_MOSI  23
#define SD_MISO  19
#define SD_SCK   18

// Indicator LED pin
#define LED_PIN 2

File file;

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = SAMPLE_BITS,
    .channel_format = CHANNEL_FORMAT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void writeWavHeader(File file, uint32_t wavSize) {
  uint32_t fileSize = wavSize + 36;
  uint16_t audioFormat = 1;
  uint16_t numChannels = 1;
  uint16_t bitsPerSample = 16;
  uint32_t byteRate = SAMPLE_RATE * numChannels * bitsPerSample / 8;
  uint16_t blockAlign = numChannels * bitsPerSample / 8;
  uint32_t sampleRate = SAMPLE_RATE;

  file.write((const uint8_t*)"RIFF", 4);
  file.write((uint8_t*)&fileSize, 4);
  file.write((const uint8_t*)"WAVE", 4);
  file.write((const uint8_t*)"fmt ", 4);

  uint32_t subchunk1Size = 16;
  file.write((uint8_t*)&subchunk1Size, 4);
  file.write((uint8_t*)&audioFormat, 2);
  file.write((uint8_t*)&numChannels, 2);
  file.write((uint8_t*)&sampleRate, 4);
  file.write((uint8_t*)&byteRate, 4);
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);

  file.write((const uint8_t*)"data", 4);
  file.write((uint8_t*)&wavSize, 4);
}

String getNextFilename() {
  char filename[16];
  for (int i = 1; i <= 999; i++) {
    snprintf(filename, sizeof(filename), "/REC%03d.wav", i);
    if (!SD.exists(filename)) {
      return String(filename);
    }
  }
  return "/REC999.wav";  // fail-safe if > 999 files exist
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Mount Failed");
    return;
  }

  String filename = getNextFilename();
  Serial.print("Saving file: ");
  Serial.println(filename);

  file = SD.open(filename.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }

  for (int i = 0; i < 44; i++) file.write((uint8_t)0);

  setupI2S();

  size_t bytesRead;
  const int totalSamples = SAMPLE_RATE * RECORD_SECONDS;
  const int bufferLen = 512;
  char buffer[bufferLen];

  Serial.println("Recording started...");
  digitalWrite(LED_PIN, HIGH);

  for (int i = 0; i < (totalSamples * 2) / bufferLen; i++) {
    i2s_read(I2S_PORT, &buffer, bufferLen, &bytesRead, portMAX_DELAY);
    file.write((uint8_t*)buffer, bytesRead);
  }

  digitalWrite(LED_PIN, LOW);
  Serial.println("Recording finished.");

  i2s_driver_uninstall(I2S_PORT);

  uint32_t dataSize = totalSamples * 2;
  file.seek(0);
  writeWavHeader(file, dataSize);
  file.close();

  Serial.println("WAV file saved.");
}

void loop() {}

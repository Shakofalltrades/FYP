# Measuring Fetal Heartbeat using MEMS microphone - USER guide

# 🩺 Placing the Non-Invasive Wearable Device on the Phantom

Ensure that all **microphones are properly positioned over the silicone incisions** on the phantom model. These placement points correspond to the areas where fetal heartbeat signals are expected to be detected.

> 🎯 Accurate placement is crucial for capturing meaningful acoustic data.

All **ESP32 boards must be powered via USB-C** connections to ensure reliable operation during recording.

---

# 🔊 Simulating the Audio Signals

1. **Connect to both audio exciter speakers via Bluetooth** using your preferred playback device.
2. Once connected, **play the corresponding fetal heartbeat signals** through each speaker.

> 💡 Use signals that replicate normal, bradycardic, or tachycardic fetal heart rates depending on your test scenario.

Ensure the playback volume and positioning mimic realistic in-womb conditions to validate microphone performance effectively.


# 👶 Fetal Heartbeat Recording and Signal Processing with ESP32

This repository provides the tools and workflow to capture and process fetal heartbeat signals using an array of 4 ESP32 microcontrollers and MATLAB signal processing techniques.

---

## 📦 Contents

- `SD_card_Recorder.ino` – Arduino sketch for recording audio to SD card on ESP32  
- `Signal Processing Framework.mlx` – MATLAB Live Script for signal processing and fetal heartbeat isolation

---

## 🚀 Setup & Execution Workflow

### 1️⃣ Upload the Recorder Firmware
Flash the [`SD_card_Recorder.ino`](./SD_card_Recorder.ino) onto **each of the 4 ESP32 boards**:

## Example using Arduino IDE or platformio
## Make sure your ESP32 board and port are selected
Upload -> SD_card_Recorder.ino -> to all 4 ESP32s

### 2️⃣ Start Audio Recording
Once uploaded:

Press the RESET button on each ESP32

This will start a 60-second audio recording

The audio is saved automatically to the SD card as a .wav file

📁 Files will be saved to the root of the SD card as:
recording_001, recording_002 etc

### 3️⃣ Retrieve the Recordings
Remove the SD cards and collect the .wav files:

### 4️⃣ Signal Processing in MATLAB
Open the MATLAB Live Script:
Signal Processing Framework.mlx

The script allows you to:

Upload the 4 ESP32 .wav recordings

Upload a reference fetal heartbeat signal

Perform signal alignment, enhancement, and attempt FHB isolation

matlab
Copy
Edit
% Inside MATLAB:
run('Signal Processing Framework.mlx')

# 🛠️ Dependencies
Arduino IDE (or PlatformIO) with ESP32 board support

MATLAB with Signal Processing Toolbox (recommended)**

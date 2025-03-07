#include "esp_camera.h"
#include "HardwareSerial.h"

#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// UART Pins (Adjust if needed)
#define TX_PIN 14  // TX of ESP32-CAM
#define RX_PIN 15  // Not used, but required for Serial.begin()

HardwareSerial espSerial(1); // Use Serial1 for UART communication

void setup() {
    Serial.begin(115200);
    espSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);  // UART Setup

    Serial.println("Initializing Camera...");
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_QVGA;
    config.pixel_format = PIXFORMAT_GRAYSCALE;  // Use grayscale for motion detection
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    // Initialize camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return;
    }
    
    Serial.println("Camera initialized successfully!");
}

bool detect_motion() {
    camera_fb_t *frame1 = esp_camera_fb_get();
    if (!frame1) {
        Serial.println("Failed to capture first frame!");
        return false;
    }

    delay(100);  // Short delay between frames

    camera_fb_t *frame2 = esp_camera_fb_get();
    if (!frame2) {
        Serial.println("Failed to capture second frame!");
        esp_camera_fb_return(frame1);
        return false;
    }

    // Compare frames
    int changes = 0;
    for (int i = 0; i < frame1->len; i++) {
        if (abs(frame1->buf[i] - frame2->buf[i]) > 50) {  // Threshold for motion
            changes++;
        }
    }

    esp_camera_fb_return(frame1);
    esp_camera_fb_return(frame2);

    return (changes > 1000);  // Adjust this threshold based on testing
}

void loop() {
    if (detect_motion()) {
        Serial.println("🚨 Motion Detected!");
        espSerial.println("MOTION_DETECTED");  // Send alert via UART
    } else {
        Serial.println("✅ No Motion.");
    }

    delay(500);
}

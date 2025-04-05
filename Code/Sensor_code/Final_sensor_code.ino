#include "HardwareSerial.h"
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// LoRa Pins
#define LORA_SCK  12
#define LORA_MISO 13
#define LORA_MOSI 11
#define LORA_NSS  10
#define LORA_RST  14
#define LORA_DIO0 2
#define LED_PIN   47  // LED Indicator

// HC-SR04 Pins
#define TRIG_PIN 3    
#define ECHO_PIN 5    

// UART (ESP32-CAM)
#define RX_PIN 44  
#define TX_PIN 43  

HardwareSerial espSerial(1);
Adafruit_MPU6050 mpu;

bool motionDetected = false;  // Motion detection flag
unsigned long lastLoRaSend = 0; // Timestamp for LoRa sending

void setup() {
    Serial.begin(115200);
    espSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

    pinMode(LED_PIN, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Initialize LoRa
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(433E6)) {  
        Serial.println("❌ LoRa init failed!");
        while (1);
    }
    Serial.println("✅ LoRa Ready.");

    // Initialize MPU6050
    Wire.begin(4, 8);  
    if (!mpu.begin()) {
        Serial.println("❌ MPU6050 not found!");
        while (1);
    }
    Serial.println("✅ MPU6050 Ready.");
}

void loop() {
    readUART();  // Check for motion detection
    sendLoRa();  // Send LoRa data every second
}

// ✅ Read Motion Detection from ESP32-CAM via UART (Non-Blocking)
void readUART() {
    if (espSerial.available()) {
        String receivedData = espSerial.readStringUntil('\n');
        receivedData.trim();
        
        if (receivedData == "MOTION_DETECTED") {
            motionDetected = true;  // Set flag when motion is detected
            Serial.println("🚨 Motion Detected!");
        }
    }
}

// ✅ Read Sensor Data and Send via LoRa (Every 1 Second)
void sendLoRa() {
    if (millis() - lastLoRaSend >= 1000) { // Send every second
        lastLoRaSend = millis();

        // Measure Distance
        float distance = getDistance();
        
        // Read MPU6050 Data
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        // Create Message
        String message = (motionDetected) ? "🚨 Motion Detected!\n" : "No Motion Detected\n";
        motionDetected = false;  // Reset motion flag after sending

        message += "Distance: " + String(distance) + " cm\n";
        message += "X: " + String(a.acceleration.x) + " m/s²\n";
        message += "Y: " + String(a.acceleration.y) + " m/s²\n";
        message += "Z: " + String(a.acceleration.z) + " m/s²";

        // Send Data via LoRa
        Serial.println("📡 Sending LoRa Data...");
        digitalWrite(LED_PIN, HIGH);
        LoRa.beginPacket();
        LoRa.print(message);
        LoRa.endPacket();
        digitalWrite(LED_PIN, LOW);

        Serial.println("✅ LoRa Data Sent:\n" + message);
    }
}

// ✅ Read Distance from HC-SR04 (Non-Blocking)
float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    return duration * 0.034 / 2;
}

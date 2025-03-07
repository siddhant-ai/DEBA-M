#include <SPI.h>
#include <LoRa.h>

// Custom ESP32-S3 LoRa Pin Definitions
#define LORA_SCK  12
#define LORA_MISO 13
#define LORA_MOSI 11
#define LORA_NSS  10
#define LORA_RST  14
#define LORA_DIO0 2

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Setup LoRa module
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(433E6)) { // Set frequency (433MHz)
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    Serial.println("LoRa transmitter initialized.");
}

void loop() {
    Serial.println("Sending message...");
    
    LoRa.beginPacket();
    LoRa.print("Hello from ESP32-S3 Test");
    LoRa.endPacket();

    Serial.println("Message sent.");
    
    delay(5000); // Send every 5 seconds
}

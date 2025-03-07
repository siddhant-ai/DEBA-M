#include <SPI.h>
#include <LoRa.h>

// Custom ESP32-S3 LoRa Pin Definitions
#define LORA_SCK  18
#define LORA_MISO 22
#define LORA_MOSI 13
#define LORA_NSS  15
#define LORA_RST  14
#define LORA_DIO0 27

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

    Serial.println("LoRa receiver initialized.");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received message: ");
        
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }

        Serial.println();
    }
}

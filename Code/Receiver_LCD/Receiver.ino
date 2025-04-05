#include <SPI.h>
#include <LoRa.h>
#include <TFT_eSPI.h> // TFT display library

// Custom ESP32-S3 LoRa Pin Definitions
#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 32
#define LORA_NSS  15
#define LORA_RST  14
#define LORA_DIO0 27

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

unsigned long previousMillis = 0;  // Store the last time the display was updated
const long interval = 1000;        // Interval to refresh display (1 second)

// To hold last printed message to avoid redundant redraw
String lastMessage = "";

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize TFT Display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);  
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    tft.setCursor(10, 10);
    tft.println("DEBA-M Sensor parameters!");

    Serial.println("TFT initialized!");

    // Setup LoRa module
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
    LoRa.setSPIFrequency(1E6); // Reduce SPI speed for stability

    if (!LoRa.begin(433E6)) { 
        Serial.println("LoRa Init Failed!");
        
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("LoRa Init Failed!");
        
        while (1);
    }

    Serial.println("LoRa initialized.");
}

void loop() {
    unsigned long currentMillis = millis();  // Get the current time

    // Handle incoming LoRa packet
    int packetSize = LoRa.parsePacket();
    
    if (packetSize) {
        String message = "";

        while (LoRa.available()) {
            char c = (char)LoRa.read();
            Serial.print(c);
            message += c;
        }
        
        Serial.println();  // Move to a new line after the message

        // Only update the display if the message is different from the last one
        if (message != lastMessage) {
            tft.setCursor(10, 40);  

            // Display received message
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.println(message);

            lastMessage = message;
        }
    }

    // Optional: Update display every second
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
    }
}

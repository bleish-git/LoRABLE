#include "heltec.h"

#define LORA_BAND 868E6

String inputBuffer = "";

void setup() {
  // inizializza OLED + Lora + Serial; paBoost = false per potenza ridotta
  Heltec.begin(true, true, true, false, LORA_BAND);
  Serial.println("TX pronto.");
}

void loop() {

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      if (inputBuffer.length() > 0) {

        // -------- DISPLAY ----------
        Heltec.display->clear();
        Heltec.display->drawString(0, 0, "TX → LoRa");
        Heltec.display->drawString(0, 12, "Msg: " + inputBuffer);
        Heltec.display->drawString(0, 24, "Freq: " + String(LORA_BAND/1e6) + " MHz");
        Heltec.display->drawString(0, 36, "Status: sent");
        Heltec.display->display();

        // -------- INVIO LORA --------
        LoRa.beginPacket();
        LoRa.print(inputBuffer);
        LoRa.endPacket();

        Serial.print("Inviato: ");
        Serial.println(inputBuffer);

        inputBuffer = "";
      }
    } else { 
      inputBuffer += c;
    }
  }
}


#include <Arduino.h>
#include "Wire.h"
#include "SSD1306.h"

void setup() {
  Serial.begin(115200);
  ecranOled.init();
  ecranOled.clear();
  ecranOled.setFont(ArialMT_Plain_16);
  ecranOled.drawString(0, 0, "Surveillance Sonore");
  ecranOled.setFont(ArialMT_Plain_10);
  ecranOled.drawString(0, 20, "Version 2024");
  ecranOled.display();
}

void loop() {
  Serial.println("*");
  delay(300);
}

SSD1306 ecranOled(0x3c, 5, 4);
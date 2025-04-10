#include <Arduino.h>
#include "Wire.h"
#include "SSD1306.h"
#include "CSon.h"
#define PERIODE_RELEVE 10000  // période relevé et envoi en ms

void setup() {
  Serial.begin(115200);
  ecranOled.init();
  ecranOled.clear();
  ecranOled.setFont(ArialMT_Plain_16);
  ecranOled.drawString(0, 0, "Surveillance Sonore");
  ecranOled.setFont(ArialMT_Plain_10);
  ecranOled.drawString(0, 20, "Version 2024");
  ecranOled.display();
  son.Setup();
}

void loop() {
  float niveauSonoreMoy=0; 
  int periodeReleve = PERIODE_RELEVE/son.tempsEchantillon;
  son.SamplesDmaAcquisition();
  Serial.print("Moyenne: "); Serial.print(son.niveauSonoreMoyen);
  Serial.print(" Crête: "); Serial.println(son.niveauSonoreCrete);
  delay(100);
}

SSD1306 ecranOled(0x3c, 5, 4);
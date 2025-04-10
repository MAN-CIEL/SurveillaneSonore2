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

void AfficherSpecte(double * vRe) {
    int barWidth = 1;
    int spacing = 0;
    int maxHeight = 63;
    ecranOled.clear();
    
    for (int i = 1; i < 128; i++) {
        double val = vRe[i];
        int barHeight = map(val, 0, 700000, 0, maxHeight);
        ecranOled.fillRect(i * (barWidth + spacing), 64 - barHeight, barWidth, barHeight);
    }
    ecranOled.display();
}

void loop() {
  static unsigned long lastSendTime = 0;
  const unsigned long sendInterval = 100; // Intervalle d'affichage en ms
  float niveauSonoreMoy=0; 
  int periodeReleve = PERIODE_RELEVE/son.tempsEchantillon;
  float niveauSonoreMoyenDB=0; 
  float niveauSonoreCreteDB=0;
  niveauSonoreMoyenDB = 20 * log10( niveauSonoreMoy )-14.56 ;   
  niveauSonoreCreteDB = 20 * log10( son.niveauSonoreCrete )-18.474 ;
  son.SamplesDmaAcquisition();
  // Affichage périodique pour éviter de surcharger le port série
  if (millis() - lastSendTime >= sendInterval) {
      lastSendTime = millis();
      
      // Version pour Serial Plotter (courbes)
      Serial.print("Moyenne_dB:"); Serial.print(niveauSonoreMoyenDB);
      Serial.print(",Crete_dB:"); Serial.println(niveauSonoreCreteDB);
      
      // Version pour Serial Monitor (valeurs précises)
      Serial.print("Niveau moyen: "); Serial.print(niveauSonoreMoyenDB); Serial.print(" dB");
      Serial.print(" | Crête: "); Serial.print(niveauSonoreCreteDB); Serial.println(" dB");
  }
  
  // Affichage sur OLED (optionnel)
  static int displayCount = 0;
  if (displayCount++ % 2 == 0) {
      AfficherSpecte(son.vReal);
  }
  delay(100);
}

SSD1306 ecranOled(0x3c, 5, 4);
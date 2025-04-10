#include "CSon.h"
#include <Wire.h>
#include "SSD1306Wire.h"

#define PERIODE_RELEVE 10000  // Période de relevé en ms

// Instanciations globales
SSD1306Wire ecranOled(0x3c, 5, 4);  // Adresse I2C, SDA, SCL
CSon son;

/**
 * @brief Affichage du spectre sur OLED
 */
void AfficherSpectre(double* vRe)
{
    ecranOled.clear();
    int maxHeight = 63;
    
    for(int i = 1; i < 128; i++) {
        int barHeight = map(vRe[i], 0, 700000, 0, maxHeight);
        ecranOled.fillRect(i, 64 - barHeight, 1, barHeight);
    }
    ecranOled.display();
}

void setup()
{
    Serial.begin(115200);
    
    // Initialisation OLED
    ecranOled.init();
    ecranOled.clear();
    ecranOled.setFont(ArialMT_Plain_16);
    ecranOled.drawString(0, 0, "Surveillance");
    ecranOled.drawString(0, 20, "Sonore");
    ecranOled.display();
    delay(2000);
    
    // Initialisation audio
    if(son.Setup() != ESP_OK) {
        Serial.println("Erreur init I2S!");
        while(1);
    }
}

void loop()
{
    static unsigned long lastTime = 0;
    static int displayCount = 0;
    
    son.SamplesDmaAcquisition();
    
    // Calcul des dB
    float moyenDB = 20 * log10(son.niveauSonoreMoyen) - 14.56;
    float creteDB = 20 * log10(son.niveauSonoreCrete) - 18.474;
    
    // Affichage série
    if(millis() - lastTime >= 100) {
        lastTime = millis();
        Serial.print("Moyen:"); Serial.print(moyenDB);
        Serial.print(",Crete:"); Serial.println(creteDB);
    }
    
    // Affichage OLED (toutes les 2 itérations)
    if(displayCount++ % 2 == 0) {
        AfficherSpectre(son.vReal);
    }
    
    delay(10);
}
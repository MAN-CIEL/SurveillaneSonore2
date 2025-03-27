#include "arduinoFFT.h" 
#include <driver/i2s.h>

#define SAMPLES 512                 // Nombre d'échantillons 
#define SAMPLING_FREQUENCY 44100    // Fréquence d'échantillonnage 
#define DMA_BUF_LEN 512             // Taille du buffer DMA : 512 échantillons 
#define DMA_BUF_COUNT 8             // Nombre de buffers DMA : 8 

/**
 * @class CSon
 * @brief Gère l'acquisition audio et les traitements associés
 */
class CSon 
{
public:
    float niveauSonoreMoyen;
    float niveauSonoreCrete;
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    esp_err_t result;
    float tempsEchantillon;
    
    CSon();
    esp_err_t Setup();
    esp_err_t SamplesDmaAcquisition();
    
private:
    size_t bytesRead;
    int32_t i2sData[SAMPLES];
    i2s_pin_config_t pinConfig;
    i2s_config_t i2sConfig;
    ArduinoFFT<double> FFT;
};
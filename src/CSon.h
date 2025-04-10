#ifndef CSON_H
#define CSON_H

#include "arduinoFFT.h"
#include <driver/i2s.h>

#define SAMPLES 512
#define SAMPLING_FREQUENCY 44100
#define DMA_BUF_LEN 512
#define DMA_BUF_COUNT 8

/**
 * @class CSon
 * @brief Classe de gestion de l'acquisition et du traitement audio
 */
class CSon {
public:
    float niveauSonoreMoyen;
    float niveauSonoreCrete;
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    esp_err_t result;
    float tempsEchantillon;
    ArduinoFFT<double> FFT;

    CSon();
    esp_err_t Setup();
    esp_err_t SamplesDmaAcquisition();

private:
    size_t bytesRead;
    int32_t i2sData[SAMPLES];
    i2s_pin_config_t pinConfig;
    i2s_config_t i2sConfig;
};

#endif
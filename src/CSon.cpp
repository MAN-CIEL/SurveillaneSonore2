#include "CSon.h"
// Configuration des broches I2S
this->pinConfig = {
    .bck_io_num = 14,   // Broche clock (BCK)
    .ws_io_num = 13,     // Broche word select (WS)
    .data_out_num = I2S_PIN_NO_CHANGE,  // Pas de sortie audio
    .data_in_num = 12    // Broche données entrantes (DIN)
};

// Configuration du bus I2S
this->i2sConfig = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),  // Mode maître en réception
    .sample_rate = SAMPLING_FREQUENCY,  // Fréquence d'échantillonnage
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,  // 32 bits par échantillon
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,  // Canal droit seulement
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,  // Pas d'interruption
    .dma_buf_count = DMA_BUF_COUNT,  // 8 buffers DMA
    .dma_buf_len = DMA_BUF_LEN,      // 512 échantillons par buffer
    .use_apl1 = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};

// Initialisation FFT
this->FFT = ArduinoFFT<double>(this->vReal, this->vImag, SAMPLES, SAMPLING_FREQUENCY);

CSon::Setup() 
{
    result = i2s_driver_install(I2S_NUM_0, &this->i2sConfig, 0, NULL); 
    result = i2s_set_pin(I2S_NUM_0, &this->pinCconfig); 
    result = i2s_zero_dma_buffer(I2S_NUM_0); 
    return result; 
}

esp_err_t CSon::SamplesDmaAcquisition()
{
    size_t bytesRead;
    // 1. Acquisition des données brutes via DMA
    result = i2s_read(I2S_NUM_0, &this->i2sData, sizeof(this->i2sData), &bytesRead, portMAX_DELAY);

    if (result == ESP_OK && bytesRead > 0)
    {
        int16_t samplesRead = bytesRead / 4;
        
        // 2. Préparation des données pour la FFT
        for (int16_t i = 0; i < samplesRead; ++i)
        {
            i2sData[i] = i2sData[i] >> 8;  // Conversion 24→32 bits
            vReal[i] = (double)i2sData[i];  // Remplissage partie réelle
            vImag[i] = 0.0;                 // Partie imaginaire à zéro
        }

        // 3. Traitement FFT (code de votre image)
        FFT.windowing(this->vReal, SAMPLES, FFT_WIN_TYPE_HAMMING, FFT_FORWARD);  // Fenêtrage
        FFT.compute(this->vReal, this->vImag, SAMPLES, FFT_FORWARD);            // Calcul FFT
        FFT.complexToMagnitude(this->vReal, this->vImag, SAMPLES);              // Conversion amplitude

        // 4. Calcul des niveaux sonores (existant)
        float mean = 0;
        for (int16_t i = 0; i < samplesRead; ++i) {
            mean += abs(i2sData[i]);
            if (abs(i2sData[i]) > niveauSonoreCrete) niveauSonoreCrete = abs(i2sData[i]);
        }
        this->niveauSonoreMoyen = mean / samplesRead;
    }

    return result;
}

/**
 * @brief Constructeur de la classe CSon
 * 
 * Initialise les attributs et configure la FFT avec :
 * - Les tableaux vReal/vImag pour le stockage
 * - Le nombre d'échantillons (SAMPLES)
 * - La fréquence d'échantillonnage (SAMPLING_FREQUENCY)
 */
CSon::CSon() 
    : FFT(vReal,               // Buffer partie réelle
          vImag,               // Buffer partie imaginaire 
          SAMPLES,             // Nombre d'échantillons
          SAMPLING_FREQUENCY)  // Fréquence d'échantillonnage
{
    /* Initialisation des niveaux sonores */
    niveauSonoreMoyen = 0.0f;
    niveauSonoreCrete = 0.0f;
    
    /* Initialisation des buffers FFT */
    memset(vReal, 0, SAMPLES * sizeof(double));
    memset(vImag, 0, SAMPLES * sizeof(double));
    
    /* Configuration I2S par défaut */
    pinConfig = {
        .bck_io_num = 14,
        .ws_io_num = 13,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = 12
    };
}

void applyAWeighting(double* signal, double* output, size_t length) {
    static double x1 = 0.0, x2 = 0.0;
    static double y1 = 0.0, y2 = 0.0;
    
    const double b[] = {0.255741125204258, -0.511482250408515, 0.255741125204258};
    const double a[] = {1.0, -1.734725768809275, 0.766006609943264};
    
    for (size_t i = 0; i < length; ++i) {
        double x0 = signal[i];
        double y0 = b[0]*x0 + b[1]*x1 + b[2]*x2 - a[1]*y1 - a[2]*y2;
        
        x2 = x1; x1 = x0;
        y2 = y1; y1 = y0;
        
        output[i] = y0;
    }
}
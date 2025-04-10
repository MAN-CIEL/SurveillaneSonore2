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

esp_err_t CSon::SampleSDmaAcquisition()
{
    // Nombre d'octets lues en mémoire DMA
    size_t bytesRead;
    
    // Capture des données audio via DMA
    // I2S_NUM_0 : Port I2S utilisé
    // &this->i2sData : Buffer de destination pour les données
    // sizeof(this->i2sData) : Taille du buffer en octets
    // &bytesRead : Nombre d'octets réellement lus (sortie)
    // portMAX_DELAY : Attente infinie si nécessaire
    result = i2s_read(I2S_NUM_0, &this->i2sData, sizeof(this->i2sData), &bytesRead, portMAX_DELAY);

    if (result == ESP_OK)
    {
        // Conversion du nombre d'octets en nombre d'échantillons (chaque échantillon fait 4 octets en 32 bits)
        int16_t samplesRead = bytesRead / 4;
        
        if (samplesRead > 0)
        {
            float mean = 0;
            
            // Traitement de chaque échantillon
            for (int16_t i = 0; i < samplesRead; ++i)
            {
                // Décalage de 8 bits vers la droite pour obtenir une valeur 24 bits signée
                // (le microphone INMP441 envoie des données 24 bits dans un format 32 bits)
                i2sData[i] = i2sData[i] >> 8;
                
                // Calcul de la valeur absolue et ajout à la moyenne
                mean += abs(i2sData[i]);
                
                // Mise à jour de la valeur crête si nécessaire
                if (abs(i2sData[i]) > niveauSonoreCrete) 
                    niveauSonoreCrete = abs(i2sData[i]);
            }
            
            // Calcul de la moyenne des valeurs absolues (approximation RMS)
            this->niveauSonoreMoyen = mean / samplesRead;
        }
    }

    return result;
}
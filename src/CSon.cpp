#include "CSon.h"

/**
 * @brief Constructeur de CSon
 */
CSon::CSon() 
    : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY),
      tempsEchantillon((float)SAMPLES / SAMPLING_FREQUENCY * 1000)
{
    niveauSonoreMoyen = 0.0f;
    niveauSonoreCrete = 0.0f;
    memset(vReal, 0, SAMPLES * sizeof(double));
    memset(vImag, 0, SAMPLES * sizeof(double));

    // Configuration I2S
    pinConfig = {
        .bck_io_num = 14,    // BCK
        .ws_io_num = 13,      // WS
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = 12     // DATA
    };

    i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLING_FREQUENCY,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
}

/**
 * @brief Initialisation du système I2S
 */
esp_err_t CSon::Setup()
{
    result = i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);
    if(result != ESP_OK) return result;
    
    result = i2s_set_pin(I2S_NUM_0, &pinConfig);
    if(result != ESP_OK) return result;
    
    return i2s_zero_dma_buffer(I2S_NUM_0);
}

/**
 * @brief Acquisition et traitement des échantillons audio
 */
esp_err_t CSon::SamplesDmaAcquisition()
{
    result = i2s_read(I2S_NUM_0, &i2sData, sizeof(i2sData), &bytesRead, portMAX_DELAY);
    
    if(result == ESP_OK && bytesRead > 0)
    {
        int16_t samplesRead = bytesRead / 4;
        float mean = 0;
        niveauSonoreCrete = 0;

        // Traitement des échantillons
        for(int i = 0; i < samplesRead; i++)
        {
            i2sData[i] >>= 8;  // Conversion 24->32 bits
            vReal[i] = (double)i2sData[i];
            vImag[i] = 0.0;

            mean += abs(i2sData[i]);
            if(abs(i2sData[i]) > niveauSonoreCrete) {
                niveauSonoreCrete = abs(i2sData[i]);
            }
        }
        niveauSonoreMoyen = mean / samplesRead;

        // Calcul FFT
        FFT.windowing(vReal, SAMPLES, FFT_WIN_TYPE_HAMMING, FFT_FORWARD);
        FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
        FFT.complexToMagnitude(vReal, vImag, SAMPLES);
    }

    return result;
}
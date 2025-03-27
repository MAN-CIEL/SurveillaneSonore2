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
uint16_t spi_adc(uint8_t channel) {

    // Check channel range (0–7 for an 8-channel ADC)
    if (channel > 7) {
        Serial.println("Invalid channel number");
        return 0xFFFF;      // return an obvious error code
    }

    // Build command bytes for the ADC (MCP3008-style)
    uint8_t channelMSB  = (channel & 0b00000100) >> 2;   // D2 bit
    uint8_t firstByte   = 0b00000110 | channelMSB;       // Start + SGL + D2
    uint8_t secondByte  = channel << 6;                  // D1, D0 into bits 7–6

    uint8_t firstByteMsk = 0x0F;                         // lower 4 bits mask

    // Start SPI transaction
    digitalWrite(CS, LOW);

    // Send first command byte
    SPI.transfer(firstByte);

    // Send second command byte and receive upper 4 data bits (D9–D6)
    uint8_t firstResult  = SPI.transfer(secondByte);

    // Send dummy byte to receive lower 8 bits (D5–D0)
    uint8_t secondResult = SPI.transfer(0x00);

    // End SPI transaction
    digitalWrite(CS, HIGH);

    // Combine the 10-bit result:
    //  - top 4 bits are in firstResult[3:0]
    //  - lower 8 bits are in secondResult[7:0]
    uint16_t adcValue = ((firstResult & firstByteMsk) << 8) | secondResult;

    return adcValue;
}

#include <SPI.h>

#define CS 5                  // Chip Select pin for the SPI ADC
#define ADC_CLK 1600000       // SPI clock speed (1.6 MHz)
#define ADC_VREF 3300         // ADC reference voltage in mV

float temp;

void setup() {
  
  pinMode(CS, OUTPUT);        // CS pin as output
  digitalWrite(CS, HIGH);     // Keep CS high when idle

  // Configure SPI mode: 1.6 MHz, MSB first, Mode 0
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();                // Initialize SPI hardware
  SPI.beginTransaction(settings);

  Serial.begin(115200);       // Start serial communication
  while(!Serial);             // Wait for serial monitor
}

void loop() {
  uint8_t ch = 0;                             // Read from channel 0
  uint16_t adc_value = spi_adc(ch);           // SPI read

  // Convert raw ADC to voltage
  float volt = 3.3 * adc_value / 4095.0;

  // Convert voltage to temperature (TMP36 formula)
  temp = (volt - 0.5) * 100;

  Serial.print("Temp in the room is ");
  Serial.print(temp, 2);
  Serial.println(" deg. C.");

  delay(1000);
}

/*
 * spi_adc:
 * Sends the required command frame to the SPI ADC,
 * reads two bytes, and reconstructs the 12-bit ADC value.
 */
uint16_t spi_adc(uint8_t channel)
{
  // Extract MSB of the channel for the command frame
  uint8_t channelMSB = (channel & 0b00000100) >> 2;

  // First and second bytes of command frame
  uint8_t firstByte = (0b00000110) | channelMSB;  
  uint8_t secondByte = channel << 6;

  uint8_t firstByteMsk = 0x0F;  // Mask for upper 4 bits of ADC result

  // Basic validation: ADC supports channels 0–7
  if (channel > 7) {
    Serial.println("Invalid Number");
    return 0xFF;
  }

  digitalWrite(CS, LOW);          // Start SPI transaction

  SPI.transfer(channelMSB);        // Send control bits
  uint8_t firstResult = SPI.transfer(secondByte);  // Read high byte
  uint8_t secondResult = SPI.transfer(0x0);        // Read low byte

  digitalWrite(CS, HIGH);         // End SPI transaction

  // Reconstruct 12-bit result
  uint16_t finalResult = (firstResult & firstByteMsk) << 8;
  finalResult |= secondResult;

  return finalResult;
}

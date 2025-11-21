#include "stm32f4xx.h"
#include "adc_config.h"

uint16_t adc_val;     // Stores the raw 12-bit ADC value (0–4095)
float voltage;        // Converted ADC voltage
float temp;           // Calculated temperature in °C

int main(void){

    adc_PA_init(4);   // Configure PA4 as an analogue input for the ADC

    // Configure PA5 as a digital output (LED pin)
    // Clear PA5 MODER bits, then set it as output mode
    GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    while(1){

        adc_val = adc_PA_read();                   // Read raw ADC value from channel on PA4
        voltage = 3.3f * adc_val / 4095.0f;        // Convert AD value to voltage (3.3V reference)
        temp = (voltage - 0.5f) * 100.0f;          // TMP36 conversion: 0.5V = 0°C, +10mV per °C

        // Optional LED control based on ADC threshold
        // if (adc_val < 2048){
        //     GPIOA->BSRR |= GPIO_BSRR_BR5;        // Turn LED OFF
        // } else{
        //     GPIOA->BSRR |= GPIO_BSRR_BS5;        // Turn LED ON
        // }

        for(int i = 0; i < 400000; i++);           // Simple delay loop (approximate timing)
    }
}

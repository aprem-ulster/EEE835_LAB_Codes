#include "stm32f4xx.h"
#include "uart_driver.h"

// External LED connected to PC5
#define LED_PIN 5

// Default blink delay (in milliseconds)
uint32_t blink_delay = 500;

/*
  delay_ms:
  Implements a millisecond delay using the SysTick counter.
  GetTick() returns the current millisecond count generated
  by the SysTick interrupt, so this loop waits until the
  requested number of milliseconds has passed.
 */
void delay_ms(uint32_t ms) {
    uint32_t start = GetTick();
    while ((GetTick() - start) < ms);
}

/*
  LED_Init:
  Configures PC5 as a digital output.
  The GPIOC clock must be enabled before modifying MODER.
 */
void LED_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;       // Enable GPIOC clock

    GPIOC->MODER &= ~GPIO_MODER_MODER5_Msk;    // Clear PC5 mode bits
    GPIOC->MODER |=  GPIO_MODER_MODER5_0;      // Set PC5 as general-purpose output
}

/*
  LED_Toggle:
  Toggles the state of PC5 by flipping the corresponding bit
  in the GPIOC output data register.
 */
static inline void LED_Toggle(void) {
    GPIOC->ODR ^= (1U << LED_PIN);
}

int main(void) {

    // Initialise external LED on PC5
    LED_Init();

    /*
      UART2 configuration:
      115200 baud, 8 data bits, no parity, 1 stop bit.
      UART2_Init() also ensures SysTick is active,
      which is required for the delay_ms function.
     */
    UART_Config_t uart2_config = {
        .baudrate    = 115200,
        .word_length = UART_WORDLENGTH_8B,
        .stop_bits   = UART_STOPBITS_1,
        .parity      = UART_PARITY_NONE
    };

    // If UART fails, blink rapidly to indicate a fault
    if (UART2_Init(&uart2_config) != UART_OK) {
        while (1) {
            LED_Toggle();
            delay_ms(100);
        }
    }

    // Send user instructions over UART
    uint8_t msg[] =
        "SysTick LED Blink - User Controlled\r\n"
        "Press 1 = 200 ms delay\r\n"
        "Press 2 = 500 ms delay\r\n"
        "Press 3 = 1000 ms delay\r\n";
    UART2_Transmit(msg, sizeof(msg) - 1, 1000);

    while (1) {

        /*
          Check if the user entered a character.
          UART2_IsDataAvailable() checks RXNE flag of USART2,
          meaning a new byte is ready to read.
         */
        if (UART2_IsDataAvailable()) {
            uint8_t ch = UART2_ReceiveByte();

            // Update blink interval based on user selection
            if (ch == '1') {
                blink_delay = 200;
                UART2_Transmit((uint8_t*)"Delay set to 200 ms\r\n", 22, 1000);
            }
            else if (ch == '2') {
                blink_delay = 500;
                UART2_Transmit((uint8_t*)"Delay set to 500 ms\r\n", 22, 1000);
            }
            else if (ch == '3') {
                blink_delay = 1000;
                UART2_Transmit((uint8_t*)"Delay set to 1000 ms\r\n", 23, 1000);
            }
        }

        // Toggle external LED with the user-selected delay
        LED_Toggle();
        delay_ms(blink_delay);
    }
}

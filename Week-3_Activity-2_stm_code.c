#include "uart_driver.h"
#include "systick_config.h"

uint8_t uart1_success = 0;   // Used to indicate if UART1 initialised correctly
UART_Status_t UART1_TX;      // Stores the status result of UART1 transmissions

int main(void){

    SysTick_Init();          // Initialise SysTick for timing and delays

    // UART1 configuration for communication with the Arduino (Serial1)
    UART_Config_t uart1_config = {
        .baudrate = 115200,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOPBITS_1,
        .word_length = UART_WORDLENGTH_8B
    };

    // UART2 configuration for communication with the PC (HTerm)
    UART_Config_t uart2_config = {
        .baudrate = 115200,
        .word_length = UART_WORDLENGTH_8B,
        .stop_bits = UART_STOPBITS_1,
        .parity = UART_PARITY_NONE,
    };

    // Initialise UART1 using the configuration above
    // Set flag for debugging if initialisation is successful
    if (UART1_Init(&uart1_config) == UART_OK){
        uart1_success = 1;
    }

    // Initialise UART2 (PC terminal interface)
    if (UART2_Init(&uart2_config) == UART_OK) {

        // Send a startup message to HTerm confirming UART2 operation
        uint8_t msg[] = "Hello, UART2!\r\n";
        UART2_Transmit(msg, sizeof(msg) - 1, 1000);

        uint8_t received_byte;

        // Main loop: monitor UART2 for incoming data
        while (1) {

            // If the PC has sent a byte through UART2
            if (UART2_IsDataAvailable()) {

                received_byte = UART2_ReceiveByte();       // Read from UART2 (PC)

                UART1_TX = UART1_TransmitByte(received_byte); // Send the same byte to Arduino via UART1

                // Return the UART1 transmission status back to the PC
                // 0 = UART_OK, other values indicate errors
                UART2_TransmitByte((uint8_t)UART1_TX);
            }
        }
    }
}

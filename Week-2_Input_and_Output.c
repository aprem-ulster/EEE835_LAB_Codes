//Task-1

#include "stm32f4xx.h"

int main(void){

    /* Enable the peripheral clocks for GPIOA and GPIOC
       ---------------------------------------------------------
       The STM32 uses a clock gating system. Before any GPIO
       pin can be configured or used, the clock for that port
       must be enabled in the AHB1ENR register.
       */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    /* Configure PC5, PC6, PC8 and PC9 as OUTPUT pins
       ---------------------------------------------------------
       Each pin has two bits in the MODER register.
       Setting MODERy_0 → 01 (binary) puts the pin in output mode.
       These pins will be used to drive the external LEDs.
       */
    GPIOC->MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 |
                    GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;

    /* Configure PA5 as OUTPUT
       ---------------------------------------------------------
       PA5 is the on-board LED (the green LED on the Nucleo).
       We set it as output so we can turn it on/off.
       */
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    /* Configure PC13 as INPUT
       ---------------------------------------------------------
       PC13 is connected to the on-board user button.
       The button is ACTIVE-LOW:
          - Not pressed → PC13 reads HIGH (1)
          - Pressed     → PC13 reads LOW  (0)
       Clearing both MODER bits sets the pin as input (00).
       */
    GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;

    /* Main loop
       ---------------------------------------------------------
       Continuously read the button state and switch the LEDs
       on or off depending on the button input.
       */
    while (1) {

        /* Read PC13 input level
           -----------------------------------------------------
           If the button is NOT pressed:
              PC13 = HIGH → (GPIOC->IDR & ID13) is TRUE
           If the button IS pressed:
              PC13 = LOW  → expression is FALSE

           LED behaviour required:
              - NOT pressed → LEDs ON
              - Pressed     → LEDs OFF
              */
        if (GPIOC->IDR & GPIO_IDR_ID13) {

            /* Button NOT pressed → turn ON all LEDs
               -------------------------------------------------
               BSRR allows atomic bit-set:
               Writing BSx sets the pin HIGH.
               */
            GPIOC->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BS6 |
                          GPIO_BSRR_BS8 | GPIO_BSRR_BS9;

            GPIOA->BSRR = GPIO_BSRR_BS5;  // Internal LED ON

        } else {

            /* Button pressed → turn OFF all LEDs
               -------------------------------------------------
               Writing BRx in BSRR resets the pin (LOW).*/

            GPIOC->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 |
                          GPIO_BSRR_BR8 | GPIO_BSRR_BR9;

            GPIOA->BSRR = GPIO_BSRR_BR5;  // Internal LED OFF
        }
    }
}


//Task-2


#include "stm32f4xx.h"

/* Simple software delay function
   ------------------------------------------------------------
   This creates a rough millisecond delay by running two loops.
   It is not precise but works well enough for visible LED timing.
   The 'volatile' keyword prevents the compiler from optimizing
   the loops away.
   ------------------------------------------------------------ */
void delay_ms(uint32_t ms) {
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 1000; j++);   // Inner loop for approximate timing
    }
}

int main(void){

    /* Enable the peripheral clocks for GPIOA and GPIOC
       ---------------------------------------------------------
       Before using any GPIO pins, the clock for each port must
       be turned on in the AHB1ENR register. This allows the
       microcontroller to access and modify the GPIO registers.
       --------------------------------------------------------- */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    /* Configure LED pins PC5, PC6, PC8, PC9 as OUTPUT
       ---------------------------------------------------------
       Each of these pins drives an external LED.
       Setting MODERy_0 = 1 configures the pin as output mode (01).
       --------------------------------------------------------- */
	GPIOC->MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 |
                    GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;

    /* Main application loop
       ---------------------------------------------------------
       Continuously check the state of the external pushbutton
       on PC10 and change the LED blinking direction accordingly.
	   */
	while(1) {

        /* Read PC10 (external pushbutton)
           -----------------------------------------------------
           The external button is wired with a pull-down resistor:
             - NOT pressed → PC10 reads LOW (0)
             - Pressed     → PC10 reads HIGH (1)

           The condition:
               !(GPIOC->IDR & GPIO_IDR_ID10) == 1
           means:
               "If PC10 is LOW" → blink left to right
           ----------------------------------------------------- */

		if(!(GPIOC->IDR & GPIO_IDR_ID10) == 1){

			/* LED sequence LEFT → RIGHT
			   -------------------------------------------------
			   Each LED is turned ON using BSx, delayed, and then
			   turned OFF using BRx in the BSRR register.
			   This produces the blinking effect in one direction.
			   */

			GPIOC->BSRR |= GPIO_BSRR_BS5;   // PC5 ON
			delay_ms(100);
			GPIOC->BSRR |= GPIO_BSRR_BR5;   // PC5 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS6;   // PC6 ON
			delay_ms(200);
			GPIOC->BSRR |= GPIO_BSRR_BR6;   // PC6 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS8;   // PC8 ON
			delay_ms(300);
			GPIOC->BSRR |= GPIO_BSRR_BR8;   // PC8 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS9;   // PC9 ON
			delay_ms(400);
			GPIOC->BSRR |= GPIO_BSRR_BR9;   // PC9 OFF

		} else {

			/* LED sequence RIGHT → LEFT
			   -------------------------------------------------
			   When the button is pressed, the blinking order
			   reverses. Delays are kept the same for visibility.*/

			GPIOC->BSRR |= GPIO_BSRR_BS9;   // PC9 ON
			delay_ms(100);
			GPIOC->BSRR |= GPIO_BSRR_BR9;   // PC9 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS8;   // PC8 ON
			delay_ms(200);
			GPIOC->BSRR |= GPIO_BSRR_BR8;   // PC8 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS6;   // PC6 ON
			delay_ms(300);
			GPIOC->BSRR |= GPIO_BSRR_BR6;   // PC6 OFF

			GPIOC->BSRR |= GPIO_BSRR_BS5;   // PC5 ON
			delay_ms(400);
			GPIOC->BSRR |= GPIO_BSRR_BR5;   // PC5 OFF
		}

        /* Short final delay before restarting the loop
           Makes the pattern easier to see.
		   */

		delay_ms(200);
	}
}

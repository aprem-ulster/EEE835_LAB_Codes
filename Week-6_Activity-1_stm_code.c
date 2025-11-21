#include "stm32f4xx.h"

// Count number of timer overflows (TIM3 is 16-bit)
volatile uint32_t tim3_overflow = 0;

// Last captured timestamp
volatile uint32_t last_capture = 0;

// Difference between two captures (in timer ticks)
volatile uint32_t period_ticks = 0;

// Converted period in microseconds
volatile uint32_t period_us = 0;

void inputCapture_TIM3CH4(void);
void TIM3_IC_callback(void);

int main(void){

    // Configure TIM3 CH4 (PC9) for input capture
    inputCapture_TIM3CH4();

    while(1){};
}

/*
 * inputCapture_TIM3CH4:
 * - Configures PC9 as TIM3_CH4 input.
 * - Sets TIM3 to capture rising edges on Channel 4.
 * - Enables update + capture interrupts.
 * - Starts TIM3.
 */
void inputCapture_TIM3CH4(void){

    // Enable GPIOC and TIM3 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // ----- Configure PC9 as Alternate Function (TIM3_CH4) -----
    GPIOC->MODER &= ~GPIO_MODER_MODER9;
    GPIOC->MODER |=  GPIO_MODER_MODER9_1;    // AF mode

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD9_Msk;
    GPIOC->PUPDR |=  GPIO_PUPDR_PUPD9_1;     // Pull-down to avoid floating input

    GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL9_Msk;
    GPIOC->AFR[1] |=  (2U << GPIO_AFRH_AFSEL9_Pos);  // AF2 = TIM3

    // ----- Configure TIM3 base timer -----
    TIM3->PSC = 1 - 1;        // Prescaler = 1 → timer runs at 16 MHz
    TIM3->ARR = 0xFFFF;       // Auto-reload (16-bit full range)

    // ----- Configure Input Capture on Channel 4 -----
    TIM3->CCMR2 |= TIM_CCMR2_CC4S_0; // CC4 mapped to input (TI4)
    TIM3->CCER  &= ~TIM_CCER_CC4P;   // Capture on rising edge
    TIM3->CCER  &= ~TIM_CCER_CC4NP;
    TIM3->CCER  |=  TIM_CCER_CC4E;   // Enable capture

    // Clear any pending flags
    TIM3->SR = 0;

    // Enable update interrupt (overflow) + capture interrupt
    TIM3->DIER |= TIM_DIER_UIE | TIM_DIER_CC4IE;

    // Generate update event to load registers
    TIM3->EGR |= TIM_EGR_UG;

    // Start TIM3 counter
    TIM3->CR1 |= TIM_CR1_CEN;

    // Enable TIM3 interrupt line in NVIC
    NVIC_EnableIRQ(TIM3_IRQn);
}

/*
 * TIM3_IRQHandler:
 * - Handles both timer overflow (UIF) and input capture (CC4IF).
 * Overflow is required so we can measure long periods beyond 16-bit.
 */
void TIM3_IRQHandler(void){

    // Timer overflow: counter passed ARR (65535)
    if (TIM3->SR & TIM_SR_UIF){
        TIM3->SR &= ~TIM_SR_UIF;
        tim3_overflow++;      // Keep track of how many wraps occurred
    }

    // Rising edge detected on CH4
    if (TIM3->SR & TIM_SR_CC4IF){
        TIM3->SR &= ~TIM_SR_CC4IF;
        TIM3_IC_callback();
    }
}

/*
 * TIM3_IC_callback:
 * - Reads captured value from CCR4.
 * - Builds a 32-bit timestamp = (overflows << 16) | capture.
 * - Computes difference between current and previous timestamp.
 * - Converts tick count to microseconds.
 */
void TIM3_IC_callback(void){

    uint32_t capture = TIM3->CCR4;   // Raw 16-bit capture

    // Build extended 32-bit timestamp using overflow count
    uint32_t timestamp = (tim3_overflow << 16) | capture;

    // Time difference between two edges
    period_ticks = timestamp - last_capture;
    last_capture = timestamp;

    // Convert timer ticks (16 MHz) → microseconds
    period_us = period_ticks / 16;
}

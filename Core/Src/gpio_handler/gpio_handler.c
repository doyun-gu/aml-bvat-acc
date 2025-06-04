#include "gpio_handler.h"

// Global Variables -------------------------------------------------
DigitalOut g_builtinLed;    // Global structure for our LED
u32 g_ledToggleTimer = 0; // Dedicated u32 timer for toggling

/**
  * @brief GPIO Initialisation Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Just enable all necessary ports
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

/**
 * @brief Digital Output ----------------------------------------------------------
 * @param pin Pin structure containing port and pin number
 * @retval None
 */

DigitalOut AssignDigitalOut (Pin_t pin) {
    DigitalOut digital_out;

    digital_out.pin = pin;
    digital_out.state = GPIO_PIN_RESET; // Initialize state to LOW

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = digital_out.pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // Set as push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // No pull-up or pull-down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // Set speed to low

    HAL_GPIO_Init(digital_out.pin.port, &GPIO_InitStruct);      // Initialize GPIO

    // HAL_GPIO_WritePin(pin.port, pin.pin, digital_out.state); // Set initial state
    WriteDigitalOut(&digital_out, 0);

    return digital_out;         // Return the initialised DigitalOut structure
}

void WriteDigitalOut(DigitalOut *digital_out, u8 state) {
    digital_out->state = state;
    HAL_GPIO_WritePin(digital_out->pin.port, digital_out->pin.pin, state);
}

void ToggleDigitalOut(DigitalOut *digital_out) {
    digital_out->state = !digital_out->state;       // Toggle state
    HAL_GPIO_TogglePin(digital_out->pin.port, digital_out->pin.pin); // Toggle GPIO pin
}

void BuiltinLED_Init(void) {
    // Assign the built-in LED to a DigitalOut structure
    g_builtinLed = AssignDigitalOut((Pin_t){D13_PORT, D13_PIN}); // Assuming D13 is the built-in LED pin
    g_builtinLed.state = GPIO_PIN_RESET; // Initialize state to LOW
    WriteDigitalOut(&g_builtinLed, 0); // Set initial state
}

/**
 * @brief Blinks the global built-in LED a specified number of times.
 * @param num_blinks Number of ON-OFF cycles.
 * @param delay_ms Delay between each toggle (half the period of one blink).
 */
void indicate_setup_with_blinks(int num_blinks, u32 delay_ms) {
    for (int i = 0; i < num_blinks * 2; i++) { // Each blink is an ON and OFF state
        ToggleDigitalOut(&g_builtinLed); // Toggle the current state
        HAL_Delay(delay_ms);             // Blocking delay
    }
    // Optionally, ensure the LED is in a known state (e.g., OFF) after indication
    WriteDigitalOut(&g_builtinLed, 0); // Assuming 0 is OFF
}

void ToggleLED(DigitalOut *led, u32 interval, u32 *last_tick_timer) {
    // Assuming timer_handler uses HAL_GetTick() or similar and updates *last_tick_timer
    // A common non-blocking timer implementation:
    // if (HAL_GetTick() - *last_tick_timer >= interval) {
    //     *last_tick_timer = HAL_GetTick(); // Update last tick time
    //     ToggleDigitalOut(led);            // Toggle the LED state
    // }

    // Or, using your existing timer_handler if it's designed correctly:
    if (timer_handler(interval, last_tick_timer)) { // Pass the dedicated u32 timer variable
        ToggleDigitalOut(led);
    }
}

/**
 * @brief Analogue Input ----------------------------------------------------------
 * @param pin Pin structure containing port and pin number
 * @retval None
 */


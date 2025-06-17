#include "gpio_handler.h"
#include "timer.h" // For timer_handler used in ToggleLED

// Global Variables Definitions -----------------------------------
DigitalOut g_builtinLed;
DigitalIn g_builtinPushButton;
u32 g_ledToggleTimer = 0;
volatile bool g_is_logging_active = false; // Definition for the global flag

/**
  * @brief GPIO Initialisation Function
  */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* --- Configure PC13 (USER Button) as an interrupt on the rising edge --- */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Trigger on rising edge (when button is pressed)
    
    // **** THIS IS THE FIX ****
    // Enable the internal pull-down resistor. This ensures the pin is held LOW when
    // the button is not pressed, preventing it from floating and triggering false interrupts.
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* EXTI interrupt init for Line 13 (which is part of 15_10) */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0); // Using a lower priority (e.g., 2) is good practice
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// --- Digital Output Functions ---

DigitalOut AssignDigitalOut (Pin_t pin) {
    DigitalOut digital_out;
    digital_out.pin = pin;
    digital_out.state = GPIO_PIN_RESET;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = digital_out.pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(digital_out.pin.port, &GPIO_InitStruct);

    WriteDigitalOut(&digital_out, 0); // Set initial state OFF
    return digital_out;
}

void WriteDigitalOut(DigitalOut *digital_out, u8 state) {
    if (digital_out && digital_out->pin.port) {
        digital_out->state = state;
        HAL_GPIO_WritePin(digital_out->pin.port, digital_out->pin.pin, (GPIO_PinState)state);
    }
}

void ToggleDigitalOut(DigitalOut *digital_out) {
    if (digital_out && digital_out->pin.port) {
        digital_out->state = !digital_out->state;
        HAL_GPIO_TogglePin(digital_out->pin.port, digital_out->pin.pin);
    }
}

void BuiltinLED_Init(void) {
    // Note: D13 is PA5 on the Nucleo-F411RE
    g_builtinLed = AssignDigitalOut((Pin_t){GPIOA, GPIO_PIN_5});
}

void indicate_setup_with_blinks(int num_blinks, u32 delay_ms) {
    for (int i = 0; i < num_blinks * 2; i++) {
        ToggleDigitalOut(&g_builtinLed);
        HAL_Delay(delay_ms);
    }
    WriteDigitalOut(&g_builtinLed, 0);
}

void ToggleLED(DigitalOut *led, u32 interval, u32 *last_tick_timer) {
    if (timer_handler(interval, last_tick_timer)) {
        ToggleDigitalOut(led);
    }
}

// --- Digital Input Functions ---

/**
 * @brief  Configures a GPIO pin as an interrupt and enables its corresponding NVIC line.
 * @note   This function is intended for dynamic configuration. Static hardware like the
 * on-board button is best configured directly in MX_GPIO_Init().
 */
void AssignDigitalIn_IT(Pin_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    IRQn_Type EXTI_IRQn;

    GPIO_InitStruct.Pin = pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN; // Set to pull-down for a button connected to 3.3V
    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

    // Determine the correct EXTI IRQn based on the pin number
    switch (pin.pin) {
        case GPIO_PIN_0:  EXTI_IRQn = EXTI0_IRQn; break;
        case GPIO_PIN_1:  EXTI_IRQn = EXTI1_IRQn; break;
        case GPIO_PIN_2:  EXTI_IRQn = EXTI2_IRQn; break;
        case GPIO_PIN_3:  EXTI_IRQn = EXTI3_IRQn; break;
        case GPIO_PIN_4:  EXTI_IRQn = EXTI4_IRQn; break;
        case GPIO_PIN_5:  case GPIO_PIN_6:  case GPIO_PIN_7:  case GPIO_PIN_8:  case GPIO_PIN_9:
            EXTI_IRQn = EXTI9_5_IRQn; break;
        case GPIO_PIN_10: case GPIO_PIN_11: case GPIO_PIN_12: case GPIO_PIN_13: case GPIO_PIN_14: case GPIO_PIN_15:
            EXTI_IRQn = EXTI15_10_IRQn; break;
        default: return;
    }

    HAL_NVIC_SetPriority(EXTI_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI_IRQn);
}

/**
 * @brief Initializes the fields of a DigitalIn software struct.
 */
void DigitalIn_Init(DigitalIn* digital_in, Pin_t pin) {
    if (digital_in == NULL) return;
    digital_in->pin = pin;
    digital_in->state = GPIO_PIN_RESET;
    digital_in->last_stable_state = GPIO_PIN_RESET;
    digital_in->reading = GPIO_PIN_RESET;
    digital_in->last_debounce_time = 0;
}


/**
 * @brief Initializes the hardware and software state for the built-in push button.
 */
void BuiltinPushButton_Init(void) {
    Pin_t button_pin = {GPIOC, GPIO_PIN_13};
    // The hardware part (setting pin to IT mode) is now done in MX_GPIO_Init()
    // This function just sets up the software struct.
    DigitalIn_Init(&g_builtinPushButton, button_pin);
}


// --- HAL GPIO Callback ---
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(u16 GPIO_Pin)
{
    static u32 last_accepted_press_time = 0;
    u32 current_time = HAL_GetTick();

    if (GPIO_Pin == g_builtinPushButton.pin.pin) {
        if (current_time - last_accepted_press_time > DEBOUNCE_DELAY_MS) {
            g_is_logging_active = !g_is_logging_active;
            if (g_is_logging_active) {
                WriteDigitalOut(&g_builtinLed, 1);
                WriteUART_Blocking("=== Data Logging STARTED ===\r\n");
            } else {
                WriteDigitalOut(&g_builtinLed, 0);
                WriteUART_Blocking("=== Data Logging STOPPED ===\r\n");
            }
            last_accepted_press_time = current_time;
        }
    }
}

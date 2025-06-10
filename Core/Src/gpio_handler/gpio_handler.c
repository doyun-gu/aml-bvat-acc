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
    // GPIO Ports Clock Enable
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // NOTE: The interrupt configuration for the USER button (PC13)
    // should be done here as it's a static part of the hardware setup.
    // This was added in a previous step but is reiterated for clarity.
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
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
    g_builtinLed = AssignDigitalOut((Pin_t){D13_PORT, D13_PIN});
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

DigitalIn AssignDigitalIn(Pin_t pin) {
    DigitalIn digital_in;
    DigitalIn_Init(&digital_in, pin); // Use the helper function

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = digital_in.pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // Changed to PULLUP for more stability
    HAL_GPIO_Init(digital_in.pin.port, &GPIO_InitStruct);

    return digital_in;
}

void AssignDigitalIn_IT(Pin_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    IRQn_Type EXTI_IRQn;

    GPIO_InitStruct.Pin = pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

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
        default: return; // No return value as function is void
    }

    HAL_NVIC_SetPriority(EXTI_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI_IRQn);
}

void DigitalIn_Init(DigitalIn* digital_in, Pin_t pin) {
    if (digital_in == NULL) return;
    digital_in->pin = pin;
    digital_in->state = GPIO_PIN_RESET;
    digital_in->last_stable_state = GPIO_PIN_RESET;
    digital_in->reading = GPIO_PIN_RESET;
    digital_in->last_debounce_time = 0;
}

void ReadDigitalIn(DigitalIn *digital_in) {
    u8 current_reading = HAL_GPIO_ReadPin(digital_in->pin.port, digital_in->pin.pin);
    if (current_reading != digital_in->reading) {
        digital_in->last_debounce_time = HAL_GetTick();
    }
    digital_in->reading = current_reading;
    if ((HAL_GetTick() - digital_in->last_debounce_time) > DEBOUNCE_DELAY_MS) {
        if (digital_in->state != digital_in->reading) {
            digital_in->state = digital_in->reading;
        }
    }
}

void BuiltinPushButton_Init(void) {
    Pin_t button_pin = {GPIOC, GPIO_PIN_13};
    // The hardware part (setting pin to IT mode) is now done in MX_GPIO_Init()
    // This function just sets up the software struct.
    DigitalIn_Init(&g_builtinPushButton, button_pin);
}


// --- HAL GPIO Callback ---

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

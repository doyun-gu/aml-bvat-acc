#include "gpio_handler.h"

// Global Variables -------------------------------------------------
DigitalOut g_builtinLed;       // Global structure for our LED
DigitalIn g_builtinPushButton; // Global structure for the push button, defined in gpio_handler.c
u32 g_ledToggleTimer = 0;      // Dedicated u32 timer for toggling

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


/**
 * @brief Digital Input ----------------------------------------------------------
 * @param pin Pin structure containing port and pin number
 * @retval None
 */

 DigitalIn AssignDigitalIn(Pin_t pin) {
    DigitalIn digital_in;

    digital_in.pin = pin;
    digital_in.state = GPIO_PIN_RESET; // Initialize state to LOW
    digital_in.reading = GPIO_PIN_RESET; // Initialize reading to LOW
    digital_in.last_debounce_time = 0; // Initialize debounce time
    digital_in.last_stable_state = GPIO_PIN_RESET; // Initialize last stable state

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = digital_in.pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     // Set as input
    GPIO_InitStruct.Pull = GPIO_NOPULL;         // No pull-up or pull-down
    HAL_GPIO_Init(digital_in.pin.port, &GPIO_InitStruct); // Initialize GPIO

    return digital_in; // Return the initialised DigitalIn structure
}

/**
 * @brief  Configures a GPIO pin as an interrupt and enables its corresponding NVIC line.
 * @note   This is a generic function to enable an interrupt on any pin at runtime.
 * It's often simpler to configure static buttons directly in MX_GPIO_Init().
 * @param  pin: The Pin_t struct defining the GPIO Port and Pin.
 * @retval None
 */
void AssignDigitalIn_IT(Pin_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    IRQn_Type EXTI_IRQn; // Variable to hold the correct IRQ number

    // Basic GPIO Configuration
    GPIO_InitStruct.Pin = pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Or IT_FALLING, or IT_RISING_FALLING
    GPIO_InitStruct.Pull = GPIO_NOPULL;         // Set to GPIO_PULLDOWN for a button connected to 3.3V
    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

    // Determine the correct EXTI IRQn based on the pin number
    switch (pin.pin) {
        case GPIO_PIN_0:
            EXTI_IRQn = EXTI0_IRQn;
            break;
        case GPIO_PIN_1:
            EXTI_IRQn = EXTI1_IRQn;
            break;
        case GPIO_PIN_2:
            EXTI_IRQn = EXTI2_IRQn;
            break;
        case GPIO_PIN_3:
            EXTI_IRQn = EXTI3_IRQn;
            break;
        case GPIO_PIN_4:
            EXTI_IRQn = EXTI4_IRQn;
            break;
        case GPIO_PIN_5:
        case GPIO_PIN_6:
        case GPIO_PIN_7:
        case GPIO_PIN_8:
        case GPIO_PIN_9:
            EXTI_IRQn = EXTI9_5_IRQn;
            break;
        case GPIO_PIN_10:
        case GPIO_PIN_11:
        case GPIO_PIN_12:
        case GPIO_PIN_13:
        case GPIO_PIN_14:
        case GPIO_PIN_15:
            EXTI_IRQn = EXTI15_10_IRQn;
            break;
        default:
            // Invalid pin for EXTI, return without configuring NVIC
            return;
    }

    // Now, use the determined IRQn to configure the NVIC
    HAL_NVIC_SetPriority(EXTI_IRQn, 0, 0); // Set desired priority
    HAL_NVIC_EnableIRQ(EXTI_IRQn);
}

/**
 * @brief Initializes the fields of a DigitalIn software struct.
 * @param digital_in: Pointer to the DigitalIn struct to be initialized.
 * @param pin: The Pin_t struct containing the port and pin to associate with this DigitalIn.
 * @retval None
 */
void DigitalIn_Init(DigitalIn* digital_in, Pin_t pin) {
    // Safety check for null pointer
    if (digital_in == NULL) {
        return;
    }

    // Initialize all the software state variables
    digital_in->pin = pin;
    digital_in->state = GPIO_PIN_RESET;
    digital_in->last_stable_state = GPIO_PIN_RESET;
    digital_in->reading = GPIO_PIN_RESET;
    digital_in->last_debounce_time = 0;
}

void ReadDigitalIn(DigitalIn *digital_in) {
    // 1. Get the current physical reading from the pin.
    u8 current_reading = HAL_GPIO_ReadPin(digital_in->pin.port, digital_in->pin.pin);

    // 2. Check if the physical reading has changed since the last time we checked.
    // This detects the start of a potential press or release event.
    if (current_reading != digital_in->reading) {
        // If the reading changed, it means the pin is potentially transitioning (bouncing).
        // We reset the debounce timer to start timing this new unstable period.
        digital_in->last_debounce_time = HAL_GetTick();
    }

    // 3. Update the last reading to be the current one for the next loop iteration.
    digital_in->reading = current_reading;

    // 4. Check if enough time has passed since the last time the reading changed.
    // This confirms the signal has been stable (not changing) for the debounce duration.
    if ((HAL_GetTick() - digital_in->last_debounce_time) > DEBOUNCE_DELAY_MS) {
        // If the pin reading has been stable for long enough, we can update our
        // official, debounced 'state'.
        if (digital_in->state != digital_in->reading) {
            digital_in->state = digital_in->reading;
            // You could add logic here to detect a "press" or "release" event if needed.
        }
    }
}

void BuiltinPushButton_Init(void) {
    // Define the pin for this specific button
    Pin_t button_pin = {GPIOC, GPIO_PIN_13};

    // 1. Configure the hardware pin to be an interrupt source
    AssignDigitalIn_IT(button_pin);

    // 2. Initialize the global software struct for this button by calling our new helper function
    DigitalIn_Init(&g_builtinPushButton, button_pin);
}

//TODO: make this logic works when toggled start logging while BUILTIN LED is ON
//TODO: Otherwise it will wait for the push button while it toggles the LED
void HAL_GPIO_EXTI_Callback(u16 GPIO_Pin)
{
    // This static variable remembers the time of the last *accepted* press.
    static u32 last_accepted_press_time = 0;
    u32 current_time = HAL_GetTick();

    // CORRECTED CHECK: Compare the incoming pin number with the pin number stored in our global struct.
    if (GPIO_Pin == g_builtinPushButton.pin.pin) {

        // DEBOUNCE LOGIC: Has enough time passed since the last accepted press?
        if (current_time - last_accepted_press_time > DEBOUNCE_DELAY_MS) {
            // If yes, this is a new, valid press. We accept it.
            
            // 1. Perform the action (toggle the state)
            g_is_logging_active = !g_is_logging_active;

            // 2. Give feedback (turn LED on/off, send UART message)
            if (g_is_logging_active) {
                WriteDigitalOut(&g_builtinLed, 1); // LED ON
                WriteUART_Blocking("=== Data Logging STARTED ===\r\n");
            } else {
                WriteDigitalOut(&g_builtinLed, 0); // LED OFF
                WriteUART_Blocking("=== Data Logging STOPPED ===\r\n");
            }

            // 3. IMPORTANT: Update the timer with the time of THIS accepted press.
            last_accepted_press_time = current_time;
        }
        // If not enough time has passed, we assume it's a bounce and do nothing.
    }
}
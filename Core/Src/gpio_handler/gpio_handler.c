#include "gpio_handler.h"

/**
  * @brief GPIO Initialization Function
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

void AssignDigitalOut (Pin pin) {
    DigitalOut digital_out;
    digital_out.pin = pin;
    digital_out.state = GPIO_PIN_RESET; // Initialize state to LOW

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = digital_out.pin.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // Set as push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // No pull-up or pull-down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // Set speed to low

    HAL_GPIO_Init(digital_out.pin.port, &GPIO_InitStruct); // Initialize GPIO

    // HAL_GPIO_WritePin(pin.port, pin.pin, digital_out.state); // Set initial state
    WriteDigitalOut(&digital_out, 0);
}

void WriteDigitalOut(DigitalOut *digital_out, u8 state) {
    digital_out->state = state;
    HAL_GPIO_WritePin(digital_out->pin.port, digital_out->pin.pin, state);
}

void ToggleDigitalOut(DigitalOut *digital_out) {
    digital_out->state = !digital_out->state;       // Toggle state
    HAL_GPIO_TogglePin(digital_out->pin.port, digital_out->pin.pin); // Toggle GPIO pin
}


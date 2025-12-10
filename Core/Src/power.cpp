#include "power.h"
#include "stm32f1xx_hal.h"
#include "adc.h"

extern uint16_t adc_value[2];
extern uint32_t loop_tick;

void setup()
{
    // Initial calibration
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value, 2);
}

void loop()
{
    if (HAL_GetTick() - loop_tick >= 5000)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        loop_tick = HAL_GetTick();
    }
}

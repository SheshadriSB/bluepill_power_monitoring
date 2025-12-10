#include "power.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "easy_crc.h"
#include "usart.h"
#include <string.h>

#define PACKET_SIZE     32
#define PAYLOAD_SIZE    30
#define START_BYTE      0xA5

static uint8_t payload_buf[PAYLOAD_SIZE];
static uint8_t payload_index = 0;

typedef struct __attribute__((packed)) {
    uint8_t start;
    uint8_t payload[PAYLOAD_SIZE];
    uint8_t crc;
} Packet32;

static Packet32 p;

extern uint16_t adc_value[3];
extern uint32_t loop_tick;


void push_adc_values()
{
    // Need 6 bytes of space (3 ADC values × 2 bytes each)
    if (payload_index + 6 > PAYLOAD_SIZE)
        return;

    payload_buf[payload_index++] = adc_value[0] >> 8;
    payload_buf[payload_index++] = adc_value[0] & 0xFF;

    payload_buf[payload_index++] = adc_value[1] >> 8;
    payload_buf[payload_index++] = adc_value[1] & 0xFF;

    payload_buf[payload_index++] = adc_value[2] >> 8;
    payload_buf[payload_index++] = adc_value[2] & 0xFF;
}


void send_packet()
{
    p.start = START_BYTE;

    // Copy payload
    for (int i = 0; i < PAYLOAD_SIZE; i++)
        p.payload[i] = payload_buf[i];

    // CRC over payload only
    p.crc = calculate_cr8x_fast(p.payload, PAYLOAD_SIZE);

    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&p, PACKET_SIZE);

    // Reset for next payload
    memset(payload_buf, 0, PAYLOAD_SIZE);
    payload_index = 0;
}


void setup()
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value, 3);
}


void loop()
{
    // Every 100 ms: add 6 bytes of ADC data
    if (HAL_GetTick() - loop_tick >= 100)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        loop_tick = HAL_GetTick();

        push_adc_values();

        // When we reach exactly 30 bytes → send
        if (payload_index >= PAYLOAD_SIZE)
            send_packet();
    }
}

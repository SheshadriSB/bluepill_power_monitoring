#include "power.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "easy_crc.h"
#include "usart.h"
#include <string.h>

#define PACKET_SIZE     32
#define PAYLOAD_SIZE    30
#define START_BYTE      0xA5

// ensure C linkage for the flag so C code can see it
extern "C" volatile uint8_t uart1_tx_done = 1; // 1 = ready at startup

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

static const float VREF = 3.3f;
static const int ADC_MAX = 4095;

// Known calibration points after resistor network
static const float V_ADC_ZERO = 1.4f;
static const float V_ADC_FULL = 2.8f;

// Precomputed counts
static const int ADC_ZERO_COUNTS = (int)((V_ADC_ZERO / VREF) * ADC_MAX + 0.5f); // ≈ 1738
static const int ADC_FULL_COUNTS = (int)((V_ADC_FULL / VREF) * ADC_MAX + 0.5f); // ≈ 3476
static const int ADC_COUNTS_SPAN = (ADC_FULL_COUNTS - ADC_ZERO_COUNTS);         // ≈ 1738

float adc_to_current(uint16_t adc_raw, float Imax)
{
    int delta = (int)adc_raw - ADC_ZERO_COUNTS;

    if (delta <= 0) return 0.0f;  // clamp negative currents
    if (ADC_COUNTS_SPAN <= 0) return 0.0f;

    float frac = (float)delta / (float)ADC_COUNTS_SPAN;

    float current = frac * Imax;

    if (current < 0.0f) current = 0.0f;
    if (current > Imax) current = Imax;

    return current;
}


void push_adc_values()
{
    if (payload_index + 6 > PAYLOAD_SIZE)
        return;

    // Convert raw ADC to current
    float current30 = adc_to_current(adc_value[0], 30.0f);  // PA1 = 30A ACS
    float current20_a = adc_to_current(adc_value[1], 20.0f); // PA2 = 20A ACS
    float current20_b = adc_to_current(adc_value[2], 20.0f); // PB0 = 20A ACS

    uint16_t mA30 = (uint16_t)(current30 * 1000);
    uint16_t mA20a = (uint16_t)(current20_a * 1000);
    uint16_t mA20b = (uint16_t)(current20_b * 1000);

    // Pack into payload
    payload_buf[payload_index++] = mA30 >> 8;
    payload_buf[payload_index++] = mA30 & 0xFF;

    payload_buf[payload_index++] = mA20a >> 8;
    payload_buf[payload_index++] = mA20a & 0xFF;

    payload_buf[payload_index++] = mA20b >> 8;
    payload_buf[payload_index++] = mA20b & 0xFF;
}



void send_packet()
{
    //If previous DMA transfer not finished, skip
    if (!uart1_tx_done)
        return;

    uart1_tx_done = 0;  // Mark DMA busy

    p.start = START_BYTE;

    // Copy payload
    for (int i = 0; i < PAYLOAD_SIZE; i++)
        p.payload[i] = payload_buf[i];

    // CRC for payload only
    p.crc = calculate_cr8x_fast(p.payload, PAYLOAD_SIZE);

    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&p, PACKET_SIZE);

    // Reset payload buffer
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

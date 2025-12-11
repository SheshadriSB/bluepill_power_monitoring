#ifndef POWER_H
#define POWER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// flag used by UART Tx complete callback
extern volatile uint8_t uart1_tx_done;

// your existing prototypes (if any)
void setup(void);
void loop(void);

#ifdef __cplusplus
}
#endif

#endif // POWER_H

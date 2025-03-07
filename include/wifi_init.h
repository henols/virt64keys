#ifndef WIFI_INIT_H
#define WIFI_INIT_H
#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and start Wi‑Fi in station mode.
 *
 * This function initializes NVS, sets up the TCP/IP stack,
 * creates the default Wi‑Fi station, registers event handlers,
 * and starts Wi‑Fi.
 */
esp_err_t wifi_init_and_start(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_INIT_H

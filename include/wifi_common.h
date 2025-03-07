#ifndef WIFI_COMMON_H
#define WIFI_COMMON_H

#include "freertos/event_groups.h"

// Global event group for Wi‑Fi events (must be created in wifi_init.c)
extern EventGroupHandle_t wifi_event_group;

// Bit used to indicate Wi‑Fi is connected.
#define WIFI_CONNECTED_BIT BIT0

#endif // WIFI_COMMON_H

#ifndef OTA_HTTP_H
#define OTA_HTTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_http_server.h"
#include "esp_err.h"

/**
 * @brief Starts the OTA HTTP server.
 *
 * This function initializes the OTA HTTP server on a predefined port and registers the OTA update URI.
 */
void start_ota_http_server(void);

/**
 * @brief Handles OTA update requests via HTTP POST.
 *
 * This function receives the firmware image, writes it to the inactive OTA partition,
 * and, upon a successful update, sets the new partition for booting.
 *
 * @param req Pointer to the HTTP request structure.
 * @return esp_err_t ESP_OK on success or an error code on failure.
 */
esp_err_t ota_post_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif // OTA_HTTP_H

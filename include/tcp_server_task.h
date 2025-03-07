#ifndef TCP_SERVER_TASK_H
#define TCP_SERVER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Task that runs a TCP server on port 6464.
 *
 * This task listens for incoming TCP connections, accepts a connection,
 * and then processes data using a TCPStream.
 *
 * @param pvParameters Task parameter (unused here).
 */
void tcp_server_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // TCP_SERVER_TASK_H

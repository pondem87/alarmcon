/*
    This class contains fumctions to connect to wifi and open a websocket connection

    Author: Tendai Pfidze
    Date: 10/12/2022
*/

#ifndef APPSOCKET_H
#define APPSOCKET_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <PrintLog.h>

#define WEBSOCKET_HOST "192.168.0.193"
#define WEBSOCKET_URL "ws/esp/"
#define WEBSOCKET_PORT 8000

#define WIFI_INDICATOR_TASK_STACK_DEPTH 4 * 1024
#define WS_LOOP_TASK_STACK_DEPTH 64 * 1024
#define CQ_SEND_TICKS_TO_WAIT 1
#define CQ_RCV_TICKS_TO_WAIT 0
#define CMDQ_SEND_TICKS_TO_WAIT 10
#define CMD_SIZE 512

class AppSocket
{
private:
    PrintLog *plog;
    const char *SSID;
    const char *WIFI_KEY;
    uint8_t wifi_indicator;
    QueueHandle_t comms_queue;    
    static void comms_loop(void *app_soc);
    static void wifi_status_loop(void* app_soc);
    uint8_t connected;
    uint8_t socket_available;
    static AppSocket* app_soc_obj;
    WebSocketsClient ws_client;
    static AppSocket* get_app_socket();
    static void web_socket_event(WStype_t type, uint8_t *payload, size_t length);
public:
    AppSocket(
        const char *ssid,
        const char *wifi_key,
        uint8_t wifi_indicator,
        PrintLog *plog
    );
    void begin();
    void send(const char* msg);
    QueueHandle_t cmd_queue;
};


#endif
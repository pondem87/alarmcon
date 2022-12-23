#include "AppSocket.h"

AppSocket::AppSocket(
    const char *ssid,
    const char *wifi_key,
    uint8_t wifi_indicator,
    PrintLog *plog)
{
    this->SSID = ssid;
    this->WIFI_KEY = wifi_key;
    this->wifi_indicator = wifi_indicator;
    this->cmd_queue = cmd_queue;
    this->plog = plog;
    this->connected = 0;
    this->socket_available = 0;
    app_soc_obj = this;
}

void AppSocket::begin()
{
    char str_buf[PQ_MAX_MSG_SIZE];
    snprintf(str_buf, PQ_MAX_MSG_SIZE, "Initiating WiFi Channel with SSID: %s", this->SSID);
    plog->println(str_buf);

    plog->println("Initiating WiFi Connection and monitoring task");

    WiFi.begin(this->SSID, this->WIFI_KEY);

    this->comms_queue = xQueueCreate(10, PQ_MAX_MSG_SIZE);
    this->cmd_queue = xQueueCreate(10, CMD_SIZE);

    xTaskCreate(
        wifi_status_loop,
        "WiFi Status Indicator",
        WIFI_INDICATOR_TASK_STACK_DEPTH,
        this,
        1,
        NULL);
    
    xTaskCreate(
        comms_loop,
        "Websocket Loop",
        WS_LOOP_TASK_STACK_DEPTH,
        this,
        1,
        NULL
    );
}

AppSocket *AppSocket::get_app_socket()
{
    return app_soc_obj;
}

/*
    wifi_status_loop tracks wifi status and updates indicator and variable
*/

void AppSocket::wifi_status_loop(void *app_soc)
{
    AppSocket *soc = (AppSocket *)app_soc;
    PrintLog *plog = soc->plog;
    uint8_t BLINKER = soc->wifi_indicator;
    pinMode(BLINKER, OUTPUT);

    soc->connected = 0;

    while (1)
    {
        if (WiFi.status() == WL_CONNECTED && !soc->connected)
        {
            plog->println("\nWiFi Connected Successfully!");
            soc->connected = 1;
            digitalWrite(BLINKER, HIGH);
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            if (soc->connected)
            {
                plog->println("\nWiFi Disconnected!");
                soc->connected = 0;
            }

            digitalWrite(BLINKER, !digitalRead(BLINKER));
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void AppSocket::comms_loop(void *app_soc)
{
    AppSocket *soc = (AppSocket *)app_soc;
    PrintLog *plog = soc->plog;
    QueueHandle_t comms_queue = soc->comms_queue;

    plog->println("Starting websocket client.");

    soc->ws_client.begin(WEBSOCKET_HOST, WEBSOCKET_PORT, WEBSOCKET_URL);

    // // event handler
    soc->ws_client.onEvent(soc->web_socket_event);

    // // use HTTP Basic Authorization this is optional remove if not needed
    // // webSocket.setAuthorization("user", "Password");

    // // try ever 5000 again if connection has failed
    soc->ws_client.setReconnectInterval(2500);

    
    plog->println("Entering websocket message loop.");

    char msg[PQ_MAX_MSG_SIZE];

    while(1)
    {
        soc->ws_client.loop();

        if (xQueueReceive(comms_queue, msg, CQ_RCV_TICKS_TO_WAIT) == pdTRUE)
        {
            // send report via socket
            if (soc->socket_available) {
                soc->ws_client.sendTXT(msg);
            }
            else
            {
                plog->println("[WSc] No websocket connection. Data discarded");
            }
            
        }

        vTaskDelay(200/portTICK_PERIOD_MS);
    }

}

void AppSocket::web_socket_event(WStype_t type, uint8_t *payload, size_t length)
{
    AppSocket *soc = get_app_socket();

    switch (type)
    {
    case WStype_DISCONNECTED:
        soc->plog->println("[WSc] Disconnected!\n");
        soc->socket_available = 0;
        break;
    case WStype_CONNECTED:
        soc->plog->println("[WSc] Successfully Connected");
        soc->socket_available = 1;
        // send message to server when Connected
        soc->ws_client.sendTXT("{\"type\":\"notification\",\"data\":{\"message\":\"websocket connected to device\",\"level\":\"info\"}}");
        break;
    case WStype_TEXT:
        soc->plog->println("[WSc] Received text data!");
        xQueueSend(soc->cmd_queue, payload, CMDQ_SEND_TICKS_TO_WAIT);
        //soc->plog->println((char*)payload);
        // send message to server
        // webSocket.sendTXT("message here");
        break;
    case WStype_BIN:
        soc->plog->println("[WSc] Received binary data!");

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

//Call this method to send data over the websocket
void AppSocket::send(const char* msg)
{
    xQueueSend(comms_queue, msg, CQ_SEND_TICKS_TO_WAIT);
}

AppSocket* AppSocket::app_soc_obj;
#include <Arduino.h>
#include <ArduinoJson.h>
#include "PrintLog.h"
#include "AppSocket.h"

#define BAUD_RATE 115200
#define WIFI_SSID "NEUTRON"
#define WIFI_KEY "heavy_neutral_particle"
#define WIFI_INDICATOR GPIO_NUM_2
#define JSON_DOC_SIZE CMD_SIZE
#define CMDQ_RCV_TICKS_TO_WAIT 1

StaticJsonDocument<JSON_DOC_SIZE> json;
char cmd_buf[JSON_DOC_SIZE];

// Functions
void parse_cmd(const char *cmd);

PrintLog plog(10, 8 * 1024);
AppSocket app_soc(WIFI_SSID, WIFI_KEY, WIFI_INDICATOR, &plog);

void setup()
{
    // put your setup code here, to run once:
    // start serial port
    Serial.begin(BAUD_RATE);

    //cmd_queue = xQueueCreate(10, JSON_DOC_SIZE);

    // start print queue
    plog.begin();
    // start wifi
    app_soc.begin();
}

void loop()
{
    // put your main code here, to run repeatedly:
    plog.print("This is how we do.");
    plog.println("Again and again...");
    app_soc.send("{\"type\":\"status\",\"data\":{\"message\":\"Running main loop...\",\"level\":\"info\"}}");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    if (xQueueReceive(app_soc.cmd_queue, cmd_buf, CMDQ_RCV_TICKS_TO_WAIT) == pdTRUE) {
        parse_cmd(cmd_buf);
    }
}

void parse_cmd(const char *cmd)
{
    deserializeJson(json, cmd);

    if (json["type"]) {
        //parse and process cmd type
        if (strcmp("action", json["type"]) == 0) {
            //check object on which action will be taken
            if (strcmp("alarm", json["data"]["object"]) == 0)
            {
                //select method for object alarm
                if (strcmp("arm", json["data"]["method"]) == 0) {
                    plog.println("Received alarm ARM command");
                } else if (strcmp("disarm", json["data"]["method"]) == 0)
                {
                    plog.println("Received alarm DISARM command");
                }
                
            }
            
        }
        else if (strcmp("notification", json["type"]) == 0)
        {
            const char* msg = json["data"]["message"];
            const char* level = json["data"]["level"];

            plog.print(level);
            plog.print(": ");
            plog.println(msg);
        }
    } else {
        plog.println("JSON object format not recognised");
    }
}
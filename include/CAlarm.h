/*

*/
#include <Arduino.h>
#include "Sensor.h"
#include "PrintLog.h"
#include "AppSocket.h"

#define MAX_SENSORS 6

class CAlarm
{
private:
    uint8_t installed_sensors;
    Sensor sensors[MAX_SENSORS];
    PrintLog *plog;
    AppSocket *app_soc;
public:
    CAlarm(
        PrintLog *plog,
        AppSocket *app_soc
        );
    void begin();
    void install_sensor(const char* name, uint8_t input_pin, uint8_t health_pin);
    void remove_sensor(const char* name);
    void config_siren(uint8_t output_pin, uint8_t health_pin);
    void arm();
    void disarm();
    void reset();
    void trigger();
    static void pin_change();
};

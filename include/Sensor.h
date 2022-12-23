/*

*/
#include <Arduino.h>

#define SENSOR_NAME_LEN 20

class Sensor
{
private:
    uint8_t input_pin;
    uint8_t health_pin;
    uint8_t input_state;
    uint8_t health_state;
    char name[SENSOR_NAME_LEN];
    
public:
    Sensor();
    void begin(
        const char* name,
        uint8_t input_pin,
        uint8_t health_pin
    );
};

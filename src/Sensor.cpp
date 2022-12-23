#include "Sensor.h"

Sensor::Sensor()
{
}

void Sensor::begin(
        const char* name,
        uint8_t input_pin,
        uint8_t health_pin
    )
{
    strncpy(this->name, name, SENSOR_NAME_LEN);
    this->health_pin = health_pin;
    this->input_pin = input_pin;

    pinMode(this->health_pin, INPUT);
    pinMode(this->input_pin, INPUT);
}
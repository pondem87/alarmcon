/*
    The PrintLog class contains functions to allow printing of messages to console.
    The class runs a task and a queue which receive and print text messages.
    This class allows multitasking using freertos.

    Author: Tendai Pfidze
    Date: 10/12/2022
*/

#ifndef PRINTLOG_H
#define PRINTLOG_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"

#define PQ_MAX_MSG_SIZE 256
#define PQ_RCV_TICKS_TO_WAIT 0
#define PQ_SEND_TICKS_TO_WAIT 1

class PrintLog
{
private:
    // handle to the freeartos queue
    // queue holds messages awaiting to be printed
    QueueHandle_t print_queue;
    // length of the freeartos queue
    size_t queue_len;
    // stack depth for task that gets messages from queue
    uint32_t task_stack_depth;
    // task function, takes PrintLog object as param
    static void print_loop(void *plog);
public:
    PrintLog(
        size_t queue_len,
        uint32_t task_stack_depth
    );
    //functions
    void begin();
    void print(const char* msg);
    void println(const char* msg);
};

#endif
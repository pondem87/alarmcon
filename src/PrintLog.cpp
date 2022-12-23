#include "PrintLog.h"

/*
    Init function sets up variables to be used by the class
*/
PrintLog::PrintLog(
    size_t queue_len,
    uint32_t task_stack_depth
)
{
    this->queue_len = queue_len;
    this->task_stack_depth = task_stack_depth;
}


/*
    begin functions starts the task and initialises the queue
*/

void PrintLog::begin()
{
    this->print_queue = xQueueCreate(queue_len, PQ_MAX_MSG_SIZE);

    xTaskCreate(
        print_loop,
        "Print Task",
        this->task_stack_depth,
        this,
        1,
        NULL
    );
}


/*
    print_loop function is run in new task
*/

void PrintLog::print_loop(void *plog)
{
    PrintLog* pl = (PrintLog*)plog;

    char msg[PQ_MAX_MSG_SIZE];

    while(1)
    {
        if (xQueueReceive(pl->print_queue, msg, PQ_RCV_TICKS_TO_WAIT) == pdTRUE)
        {
            Serial.print(msg);
        }

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


/*
    println and print functions are called by other classes to send messages for printing
*/
void PrintLog::println(const char* msg)
{
    char temp[PQ_MAX_MSG_SIZE];
    snprintf(temp, PQ_MAX_MSG_SIZE, "%s\n", msg);
    print(temp);
}

void PrintLog::print(const char* msg)
{
    xQueueSend(print_queue, msg, PQ_SEND_TICKS_TO_WAIT);
}
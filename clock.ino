/*
 * Example of a basic FreeRTOS queue
 * https://www.freertos.org/Embedded-RTOS-Queues.html
 */

// Include Arduino FreeRTOS library
#include <Arduino_FreeRTOS.h>

// Include queue support
#include <queue.h>

/* 
 * Declaring a global variable of type QueueHandle_t 
 * 
 */
QueueHandle_t integerQueue;

void setup() {

  /**
   * Create a queue.
   * https://www.freertos.org/a00116.html
   */
  integerQueue = xQueueCreate(10, // Queue length
                              sizeof(int) // Queue item size
                              );
  
  if (integerQueue != NULL) {

    // Create task that publish data in the queue if it was created.
    xTaskCreate(TaskSwitch, // Task function
                "Dip Switches", // Task name
                128,  // Stack size
                NULL, 
                0, // Priority
                NULL);
    
  }


  xTaskCreate(TaskBlink, // Task function
              "Blink", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              NULL );

}

void loop() {}


void TaskSwitch(void * pvParameters) {
  (void) pvParameters;

  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);

  int del = 0;
  
  for (;;) 
  {
    del = digitalRead(7) + 2*digitalRead(8) + 4*digitalRead(9) + 8*digitalRead(10);
    xQueueSend(integerQueue, &del, portMAX_DELAY);
    vTaskDelay(1);
  }
}

/* 
 * Blink task. 
 * See Blink_AnalogRead example. 
 */
void TaskBlink(void *pvParameters)
{
  (void) pvParameters;

  pinMode(12, OUTPUT);

  int del = 0;

  int valueFromQueue;
  
  for (;;)
  {
    if (xQueueReceive(integerQueue, &valueFromQueue, portMAX_DELAY) == pdPASS) {
      del = 10 * valueFromQueue;
    }
    digitalWrite(12, HIGH);
    vTaskDelay( del / portTICK_PERIOD_MS );
    digitalWrite(12, LOW);
    vTaskDelay( del / portTICK_PERIOD_MS );
  }
}

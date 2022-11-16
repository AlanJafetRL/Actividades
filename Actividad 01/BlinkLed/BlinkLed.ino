#include <Arduino.h> // Si utiliza el entorno de Arduino, esta linea debera ser comentada
/**
 * Tarea que realiza el parpadeo de un LED
 */

// Configuracion de nucleos a utilizar 
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Numero de GPIO a utilizar 
static const int led_pin = 14;

// Tarea: blink LED
void toggleLED(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {

  // Configuracion de GPIO como salida
  pinMode(led_pin, OUTPUT);

  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              toggleLED,    // Function to be called
              "Toggle LED", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,         // Parameter to pass to function
              1,            // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,         // Task handle
              app_cpu);     // Run on one core for demo purposes (ESP32 only)

}
void loop() {
}

/*
--- Funcionamiento---
Se crea primero una función que encenderá y apagará un led con un delay de 500ms.
La tarea se le asigna a una prioridad de 1 y corre con 1 o 2 núcleos dependiendo
de la configuración de freertos.
El while dentro de la función permite que la tarea se repita.
*/
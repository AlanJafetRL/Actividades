//#include <timers.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Variables globales
static TimerHandle_t one_shot_timer = NULL;
static TimerHandle_t auto_reload_timer = NULL;

//*****************************************************************************
// Callbacks

// La función es llamada cuando el timer expira
void myTimerCallback(TimerHandle_t xTimer) {

  // Mensaje del timer 0
  if ((uint32_t)pvTimerGetTimerID(xTimer) == 0) {
    Serial.println("One-shot timer expired");
  }

  // Mensaje del timer 1
  if ((uint32_t)pvTimerGetTimerID(xTimer) == 1) {
    Serial.println("Auto-reload timer expired");
  }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

  // Configure Serial
  Serial.begin(115200);

  // retardo 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Timers---");

  // Timer Simple tiro
  one_shot_timer = xTimerCreate(
                      "One-shot timer",           // Name of timer
                      2000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
                      pdFALSE,                    // Auto-reload
                      (void *)0,                  // Timer ID
                      myTimerCallback);           // Callback function

  // Timer con reiniciable 
  auto_reload_timer = xTimerCreate(
                      "Auto-reload timer",        // Name of timer
                      1000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
                      pdTRUE,                     // Auto-reload
                      (void *)1,                  // Timer ID
                      myTimerCallback);           // Callback function

  // Garantizamos que fue creado
  if (one_shot_timer == NULL || auto_reload_timer == NULL) {
    Serial.println("Could not create one of the timers");
  } else {
    
    // mensaje inicial 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting timers...");

    // Start timers
    xTimerStart(one_shot_timer, portMAX_DELAY);
    xTimerStart(auto_reload_timer, portMAX_DELAY);
  }

  // Delete self task to show that timers will work with no user tasks
  vTaskDelete(NULL);
}


void loop() {
  // Execution should never get here
}

/*
--Funcionamiento--
El timer se utiliza para lanzar una tarea después cierto tiempo definido en ticks. Pueden hacerse
bucles haciendo que el timer se reinicie.
En este código, se inician 2 timers, uno que sólo imprime su mensaje una vez y otro que se auto-recarga.
Si por alguna razón, algún timer no se crea o no se llama correctamente, se avisa en consola.
*/
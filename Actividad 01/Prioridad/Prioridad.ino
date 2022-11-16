#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int rate_1 = 1000;  // ms
static const int rate_2 = 500;  // ms
static const int led_pin = 14;

void toggleLED_1(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
  }
}

void toggleLED_2(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);
  
  xTaskCreatePinnedToCore(  
              toggleLED_1,  
              "Toggle 1",   
              1024,         
              NULL,         
              0,            
              NULL,         
              app_cpu);     

  xTaskCreatePinnedToCore(  
              toggleLED_2,  
              "Toggle 2",   
              1024,         
              NULL,         
              0,            
              NULL,         
              app_cpu);     
}

void loop() {
}

/*
---Funcionamiento---
Cuando la prioridad de Toggle1 es 0 y la de Toggle2 es 1, el led enciende cada 500ms.
Cuando la prioridad de Toggle1 es 1 y la de Toggle2 es 0, el led enciende cada 1000ms. Por tanto, podemos decir que se ejecuta la tarea que tenga prioridad 1.

Cuando la prioridad de los dos Toggle es 1 se combinan las tareas y se interrumpen entre ellas. Esto causa conflicto en el nucleo del procesador al tratar de liberarse.
Sabiendo que el vTaskDelay se utiliza para bloquear la tarea por un tiempo para poder liberar el procesador de la tarea y, utilizando delays que no se sincronicen entre las tareas,
por ejemplo, Toggle1 1000ms y Toggle2 750ms, llegará el punto en que las dos tareas se desbloquearán a destiempo y se ejecutarán de manera errática sin respetar ningun delay.

Cuando la prioridad de los dos Toggle es 0 no se ejecuta ninguna de las tareas.
*/

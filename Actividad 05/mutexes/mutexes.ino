// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Globals
static int shared_var = 0;
static SemaphoreHandle_t mutex;

// Tasks

// Increment shared variable (the wrong way)
void incTask(void *parameters) {

  int local_var;

  // Loop forever
  while (1) {

    // Take mutex prior to critical section
    if (xSemaphoreTake(mutex, 0) == pdTRUE) {

      local_var = shared_var;
      local_var++;
      vTaskDelay(random(100, 500) / portTICK_PERIOD_MS);
      shared_var = local_var;
      
      Serial.println(shared_var);
  
      xSemaphoreGive(mutex);

    } else {
      // Do something else
    }
  }
}


void setup() {

  // Hack to kinda get randomness
  randomSeed(analogRead(0));

  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS mutexes-");

  mutex = xSemaphoreCreateMutex();

  // Start task 1
  xTaskCreatePinnedToCore(incTask,
                          "Increment Task 1",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Start task 2
  xTaskCreatePinnedToCore(incTask,
                          "Increment Task 2",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);           

  vTaskDelete(NULL);
}

void loop() {
  
}
/*
--Funcionamiento--
Mutex se utiliza para guardar recursos y que puedan ser accesados por varias tareas. Para que 
una tarea pueda accesar a los recursos de mutex cuando otra ya lo está haciendo, es necesario que 
espere por unos ticks hasta que mutex se encuentre libre, es un funcionamiento similar a los queue, pero
ahora permitiendo que varias tareas puedan esperar a mutex sin provocar una cola llena como con queue.
El programa es un simple contador que va incrementando su valor e imprimiendolo con retardos 
aleatorios que van de los 100 a los 500ms
*/
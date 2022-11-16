// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Mensaje inicial 
const char msg[] = "Mensaje inicial --- UPIIZ ---";

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//*****************************************************************************
// Tareas

// Tarea con baja prioridad
void startTask1(void *parameter) {

  // Conteo de numero de caracteres 
  int msg_len = strlen(msg);

  // Imprimir mensaje por terminal
  while (1) {
    Serial.println();
    for (int i = 0; i < msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Tarea con alta prioridad
void startTask2(void *parameter) {
  while (1) {
    Serial.print('*');
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main 
 
void setup() {

  // Serial lento para poder visualizar 
  Serial.begin(300);

  // Retardo antes de iniciar 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS UPIIZ ---");

  // Print self priority
  Serial.print("Configuracion y ciclo de tareas corriendo ");
  Serial.print(xPortGetCoreID());
  Serial.print(" con prioridad ");
  Serial.println(uxTaskPriorityGet(NULL));

 // Task corre por siempre
  xTaskCreatePinnedToCore(startTask1,
                          "Tarea 1",
                          1024,
                          NULL,
                          1,
                          &task_1,
                          app_cpu);

  // Tarea con alta prioridad corre solo por una vez 
  xTaskCreatePinnedToCore(startTask2,
                          "Tarea 2",
                          1024,
                          NULL,
                          2,
                          &task_2,
                          app_cpu);
}

void loop() {

  // Suspendemos la tarea de alta prioridad por algunos periodos de tiempo
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  // Borramos tarea de baja prioridad 
  if (task_1 != NULL) {
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}

/*
---Funcionamiento---
El funcionamiento principal del código es crear 2 tareas con distinta prioridad, pero luego de un periodo de tiempo detiene una tarea.
Primero se ejecuta la Tarea 1 que imprime en el monitor lo que contiene la cadena de caracteres msg[] y la Tarea 2 que sólo imprime asteriscos.
Luego entra un contador de suspensión de tarea que suspende y reanuda la Tarea 2 cada 2segundos. Luego de eso, la Tarea 2 se sigue ejecutando.
Por otro lado, cuando el parámetro de retorno de la Tarea 1 es distinto a NULL se elimina dicha tarea para sólo dejar ejecutando la Tarea 1.
*/
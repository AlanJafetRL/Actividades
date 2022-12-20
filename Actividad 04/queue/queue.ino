// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t msg_queue_len = 1; //Valor a modificar para reducir cuántas tareas en cola puede haber
//=15 y 0 resula en errores y se reinicia la placa
//=1 hasta 14 sigue el programa con normalidad

// Globals
static QueueHandle_t msg_queue;

//**************************************************************************
// Tasks

void printMessages(void *parameters) {

  int item;

  // Loop forever
  while (1) {

    
    if (xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE) {
    }
    Serial.println(item); //Apunta a num e imprime

    // Wait before trying again
    vTaskDelay(500 / portTICK_PERIOD_MS); 
  }
}

// Main 

void setup() {

  // Configure Serial
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Queue Demo---");

  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  // Start print task
  xTaskCreatePinnedToCore(printMessages,
                          "Print Messages",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
}

void loop() {

  static int num  = 0;

  if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE) {
    Serial.println("Queue full 1er msg");
  }
  num++;
  
  if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE) {
    Serial.println("Queue full 2do msg");
  }
  
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/*
--Funcionamiento--
Para llenar la cola es necesario saturar con varias tareas. Es más facil reducir la cola a 1 para
saturar la cola con 2 tareas. Si hacemos que a la cola se le envíen 2 mensajes casi al mismo tiempo,
una de las tareas tendrá que esperar cierto número de ticks para volver a enviar el mensaje, esta espera 
sirve para mantenerse en cola y enviar el mensaje cuando la cola esté libre.
Si se hiciera otra tarea parecida a la de printMessages utilizando la misma cola, tendriamos el mismo
problema; la cola se llenaría. Para solucionarlo se tendría que hacer una cola más grande u otra cola nueva.
*/
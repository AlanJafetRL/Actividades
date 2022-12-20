// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Configuracion 
static const uint8_t buf_len = 255;

// variables globales
static char *msg_ptr = NULL;
static volatile uint8_t msg_flag = 0;

//*****************************************************************************
// Tareas

// Tareas: lectura desde el buffer del serial
void readSerial(void *parameters) {

  char c;
  char buf[buf_len];
  uint8_t idx = 0;

  // Limpieza de buffer
  memset(buf, 0, buf_len);
  
  // Loop infinito
  while (1) {

    // Lectura de caracteres del serial
    if (Serial.available() > 0) {
      c = Serial.read();

      // Almacenamos caracteres recibidos en buffer si no sobre pasa el límite del buffer 
      if (idx < buf_len - 1) {
        buf[idx] = c;
        idx++;
      }

      // Creamos el buffer para imprimir el mensaje
      if (c == '\n') {

        // Ultimo caracter '\n', se reemplaza por un caracter nulo
        // '\0'
        buf[idx - 1] = '\0';

        // Intenta ubicar el mensaje en la localidad de memoria, si esta en uso lo ignora.
        if (msg_flag == 0) {
          msg_ptr = (char *)pvPortMalloc(idx * sizeof(char));

          // Si regresa un cero entonces se borra
          configASSERT(msg_ptr);

          // Copiamos mensaje 
          memcpy(msg_ptr, buf, idx);

          // Notificacion de que esta listo el mensaje
          msg_flag = 1;
        }

        // Reinicio de buffer y el puntero
        memset(buf, 0, buf_len);
        idx = 0;
      }
    }
  }
}

// Tarea: imprime mensaje siempre y cuando la bandera este en alto y el buffer libre 
void printMessage(void *parameters) {
  while (1) {

    // Esperamos bandera
    if (msg_flag == 1) {
      Serial.println(msg_ptr);

      // Buffer libre, ponemos puntero en caracter nulo y limpiamos bandera de notificacion
      vPortFree(msg_ptr);
      msg_ptr = NULL;
      msg_flag = 0;
    }
  }
}

//**************************************************************************
// Main 
void setup() {

  // 
  Serial.begin(115200);

  // 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS ---");
  Serial.println("Actividad 03");

  // Start Serial receive task
  xTaskCreatePinnedToCore(readSerial,
                          "Lectura Serial",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Start Serial print task
  xTaskCreatePinnedToCore(printMessage,
                          "Escritura Mensaje",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  
  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // Execution should never get here
}


/*
---Funcionamiento---
Se tienen 2 tareas, una que lee lo que recibe el puerto serial y otra que imprime en el monitor
lo que se recibió. Sin embargo, la tarea dos (de escritura) no envía nada al monitor hasta que una bandera
en la tarea de lectura indica que se ha leido el mensaje desde la memoria buffer.
El tamaño del buffer es de 255 bits, por tanto, si la cadena de caracteres enviada supera los
255 bits, esta no se guardará y no se procesará
*/
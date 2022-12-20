// Semaforo binario

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// defina el led de su eleccion
static const int led_pin = /*Aqui*/;

// variable global
static SemaphoreHandle_t bin_sem;

//***************************************************************************
// Tareas

// Blink LED con base a la tasa predefinida 
void blinkLED(void *parameters) {

  // copiamos parametros en variables globales 
  int num = *(int *)parameters;

  // Liberamos semaforo binario 
  xSemaphoreGive(bin_sem);

  // imprimimos parametros 
  Serial.print("Recibimos un: ");
  Serial.println(num);

  // LED como salida 
  pinMode(led_pin, OUTPUT);

  // ciclo infinito 
  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(num / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main 

void setup() {

  long int delay_arg;

  // Configure Serial
  Serial.begin(115200);

  // retardo antes de iniciar 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS ---");
  Serial.println(" Introducir el valor del retardo en milisegundos ");

  // esperamos por el valor 
  while (Serial.available() <= 0);

  // leemos el dato y lo almacenamos 
  delay_arg = Serial.parseInt();
  Serial.print("Enviamos: ");
  Serial.println(delay_arg);
  
  // Creamos semaforo binario 
  bin_sem = xSemaphoreCreateBinary();

  // tarea 1
  xTaskCreatePinnedToCore(blinkLED,
                          "Blink LED",
                          1024,
                          (void *)&delay_arg,
                          1,
                          NULL,
                          app_cpu);

  // 
  xSemaphoreTake(bin_sem, portMAX_DELAY);

  // 
  Serial.println("Done!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
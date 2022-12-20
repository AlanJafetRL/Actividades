//#include <timers.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Variables globales
static TimerHandle_t reload_timer = NULL;
static char char_aux[254];
char incomingChar;
int cont_char;

//*****************************************************************************
//Utilizando parte del códigos realizado en Actividad03-Ejercicio_02 
//Lectura
void lectura (void *parameter){
	cont_char=0;
	while (1){
		if (Serial.available()>0){
			incomingChar = Serial.read();
			if (cont_char < 254){
				char_aux[cont_char]=incomingChar;
				Serial.print(incomingChar);
				cont_char++;
			}
			if (incomingChar=='\n'){
				xTimerStart(reload_timer, portMAX_DELAY);
				cont_char=0;
			}
		}
	}
}

// Callbacks
// La función es llamada cuando el timer expira
void myTimerCallback(TimerHandle_t xTimer) {
	// Mensaje del timer 1
	if ((uint32_t)pvTimerGetTimerID(xTimer) == 1) {
		Serial.println("Timer expiro");
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

	// Timer con reiniciable 
	reload_timer = xTimerCreate(
							"Timer expiro",       		// Name of timer
							1000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
							pdFALSE,                    // Auto-reload
							(void *)1,                  // Timer ID
							myTimerCallback);           // Callback function

	//Tarea 1. Lectura desde serial 
	xTaskCreatePinnedToCore(lectura,
							"Lectura Serial",
							1024,
							NULL,
							1,
							NULL,
							app_cpu);

	// Garantizamos que fue creado
	if (reload_timer == NULL) {
		Serial.println("No se pudo iniciar el timer");
	} else {
		// mensaje inicial 
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		Serial.println("Ingrese cualquier caracter para reiniciar el timer...");
	}

	// Delete self task to show that timers will work with no user tasks
	vTaskDelete(NULL);
}


void loop() {

}

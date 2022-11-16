#include <Arduino.h> 

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Numero de GPIO a utilizar 
static const int led_pin = 4;
// Variable a controlar
int vel=1000;
int vel_aux=0;

//*****************************************************************************
// Tareas

// Tarea: blink LED
void toggleLED(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(vel / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(vel / portTICK_PERIOD_MS);
  }
}

// Tarea: lectura monitor
void lect_Moni(void *parameter) {
	String numero;
	while(1) {
		if (Serial.available()){
			numero=Serial.readStringUntil('\n');
			vel_aux=numero.toInt();
			if (vel_aux>0) {
				vel=vel_aux;
				Serial.print("Velocidad actual: ");
				Serial.println(numero+"ms");
			} else{
				Serial.println("ERROR: Ingrese un número válido");
			}
		}
	}
}

//*****************************************************************************
// Main 
 
void setup() {
  Serial.begin(115200);
  
  // Configuracion de GPIO como salida
  pinMode(led_pin, OUTPUT);

  // Retardo antes de iniciar 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--Ingrese un número entero para parpadear el LED integrado a esa velocidad--");
  Serial.println("Velocidad actual: 1000ms");
  
 // Tarea con baja prioridad, corre por siempre
  xTaskCreatePinnedToCore(lect_Moni,
                          "Lectura de Monitor",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Tarea con alta prioridad, corre por siempre
  xTaskCreatePinnedToCore(toggleLED,
                          "Parpadear LED",
                          1024,
                          NULL,
                          2,
                          NULL,
                          app_cpu);
}

void loop() {

}

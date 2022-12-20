// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//--Variables--//
static volatile uint8_t flag;
static char char_aux[254];

char incomingChar;
int cont_char;

//--Tareas--//

//Lectura
void lectura (void *parameter){
	flag=0;
	cont_char=0;
	while (1){
		if (Serial.available()>0){
			incomingChar = Serial.read();
			if (flag==0){
				if (cont_char < 254){
					char_aux[cont_char]=incomingChar;
					cont_char++;
				}
				if (incomingChar=='\n'){
					flag=1;
					cont_char=0;
				}
			}
		}
	}
}

//Escritura en monitor
void escritura (void *parameter) {	
	while(1){
		if (flag==1){
			Serial.print(char_aux);
			flag=0;
		}
	}
}

//--Main--//
void setup(){
	Serial.begin(115200);
	vTaskDelay(1000/portTICK_PERIOD_MS);
	Serial.println();
	Serial.println("---FreeRTOS ---");
    Serial.println("Actividad 03. Localidades de memoria");
	
	//Tarea 1. Lectura desde serial 
	xTaskCreatePinnedToCore(lectura,
							"Lectura Serial",
							1024,
							NULL,
							1,
							NULL,
							app_cpu);

	//Tarea 2. Escritura de mensaje
	xTaskCreatePinnedToCore(escritura,
							"Escritura Mensaje",
							1024,
							NULL,
							1,
							NULL,
							app_cpu);
	
	// Delete "setup and loop" task
	vTaskDelete(NULL);
}

void loop(){
	
}


/*
--Funcionamiento--
El código está basado en el que se facilitó, sólo se elimina el uso del buffer simplificando un poco
la comprensión del código. También se hace uso de menos variables.
*/
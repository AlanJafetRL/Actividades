#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

//---Bluetooth---//
const int led = 13;
const int btn_bt = 14;
int edo_btn=1;
int edo_btn_new;
int cont=0;
int BTHControl=0; 
int BTCont=0;

BluetoothSerial BT;
void ConfigByBluetooth(void);

void setup() {
	Serial.begin(115200);	
	pinMode(btn_bt, INPUT_PULLUP);
	pinMode(led, OUTPUT);
}

void loop() {
	while(1) {
		//Lee el botón  
		edo_btn_new=digitalRead(btn_bt);
		if (edo_btn==0 && edo_btn_new==1) {
			if (cont==0){
				Serial.println("Bluetooth Encendido por Boton");
				cont=1;
			}else{
				Serial.println("Bluetooth Apagado por Boton");
				cont=0;
				BT.end();
				BTCont=0;
			}
		}
		edo_btn=edo_btn_new;
		//Sincronizar contador y control para encendido y apagado de BT
		
		BTHControl=cont;
		ConfigByBluetooth();		
	}
	
}


void ConfigByBluetooth(void){
	//Inicializamos bluetooth
	String message = "";
	char incomingChar;
	if (BTHControl == 1){
		//Inicia BT
		if (BTCont==0) {
			BT.begin("Kratos uwu."); 
			Serial.println("Dispositivo listo");
			BTCont=1;
		}
		
		if(BT.available()){
			incomingChar = BT.read();
			if(incomingChar != '\n'){
				message += String(incomingChar);
			}else{
				message = "";
			}
			Serial.print(incomingChar);
		}
		
		//Encender apagar LED
		if(message == "1"){
			digitalWrite(led, HIGH); 
			BT.println("LED encendido");
		} else if (message == "0") {
			digitalWrite(led, LOW);
			BT.println("LED apagado");
		} 
		
		//Apagar con aplicación
		if(message == "R"){
			BT.println("Apagando BT");
			delay(250);
			Serial.println("\nBluetooth Apagado por Aplicacion");
			BT.println("Bluetooth Apagado por Aplicacion");
			BTHControl=0;
			cont=0;
			BT.end();
		}
		
	}
}



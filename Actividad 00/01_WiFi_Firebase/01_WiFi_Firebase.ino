#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include "FirebaseESP32.h"  //incluye libreria Firebase ESP32

const char WIFI_SSID[] = "INFINITUM92A4_2.4";
const char WIFI_PASSWORD[] = "A63sfXE62Y";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

//---Firebase---//
//Credenciales WiFi
#define WIFI_SSID "INFINITUM92A4_2.4"
#define WIFI_PASSWORD "A63sfXE62Y"

//Credenciales Proyecto Firebase
#define FIREBASE_HOST "https://eval01-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "SHUrgfvosQbAEnewYURrBh1ytvTLgw7cdhptaNt7"

//Firebase Data Object
FirebaseData firebaseData;

//Rutas
String nodo1 = "/Fecha";
String nodo2 = "/SensorCm";
String nodo3 = "/SensorIn";

/* Ultrasonido */
#define SOUND 0.01723
#define CM_TO_INCH 0.393701

const int trigPin = 14;
const int echoPin = 2;  
long duration;
float distanceCm;
float distanceInch;
/*---------------------*/


String fecha = "0";
void GetDate(void);
void ReadUltrasonico(void);

// Variable de control
int control = 0; 


void setup() {  
  Serial.begin(9600); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conectando a red Wi-Fi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
  control = 1; 
  Serial.println("");
  Serial.print("Conectado con la siguiente direccion IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Salida de escritura en firebase tiny=1s
  Firebase.setwriteSizeLimit(firebaseData, "tiny"); 
}


void loop() {  
    while(1){
        //Obtener fecha
        GetDate();
        ReadUltrasonico();
        
        //Escribir datos
        Firebase.setString(firebaseData, nodo1, fecha);
        Firebase.setFloat(firebaseData, nodo2, distanceCm);
        Firebase.setFloat(firebaseData, nodo3, distanceInch);
        
        delay(1000);
    }
}


void GetDate(void){
  struct tm timeinfo;
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  char timeYear[5];
  strftime(timeYear,5, "%Y", &timeinfo);
  char timeMonth[15];
  strftime(timeMonth,15, "%B", &timeinfo);
  char timeDay[10];
  strftime(timeDay,10, "%d", &timeinfo);
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  char timeMin[3];
  strftime(timeMin,3, "%M", &timeinfo);
  fecha = String(timeYear)+"/"+String(timeMonth)+"/"+String(timeDay)+" - "+String(timeHour)+":"+String(timeMin);
  Serial.println(fecha);
}


void ReadUltrasonico(void){
  //Inicializa el pin del emisor en salida
  pinMode(trigPin, OUTPUT);
  //Apaga el emisor
  digitalWrite(trigPin, LOW);
  //Retrasa la emisión por 2 us
  delayMicroseconds(2);
  //Comienza a emitir sonido
  digitalWrite(trigPin, HIGH);
  //Retrasa la emisión por 10 us
  delayMicroseconds(10);
  //Apaga el emisor de sonido
  digitalWrite(trigPin, LOW);
  //Escucha el sonido
  pinMode(echoPin, INPUT);
  //Calcula la duración
  duration = pulseIn(echoPin, HIGH);
  
  //Calcula la distancia
  distanceCm = duration*SOUND;  
  distanceInch = distanceCm*CM_TO_INCH;
   
  //Imprime en Serial Monitor
  Serial.print("Duracion: ");
  Serial.println(duration);
  
  Serial.print("Distancia (cm): ");
  Serial.println(distanceCm);

  Serial.print("Distancia (in): ");
  Serial.println(distanceInch);
}

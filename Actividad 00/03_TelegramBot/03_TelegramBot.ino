#ifdef ESP32
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "INFINITUM92A4_2.4";
const char* password  = "A63sfXE62Y";
#define BOTtoken "5525406636:AAHH9uXlm_keLfPIjqA9Hg-37MIPaqzTsR4"
#define CHAT_ID "367724664"

#ifdef ESP8266
	X509List cert (TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot (BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 14;
bool ledState = LOW;

void handleNewMessages(int numNewMessages){
	Serial.println("handleNewMessages");
	Serial.println(String(numNewMessages));
	
	for (int i=0; i<numNewMessages; i++){
		String chat_id = String(bot.messages[i].chat_id);
		if (chat_id != CHAT_ID){
			bot.sendMessage(chat_id, "Unauthorized user", "");
			continue;
		}
		
		String text = bot.messages[i].text;
		Serial.println(text);
		
		String from_name = bot.messages[i].from_name;
		
		if (text=="/start"){
			String welcome = "Bienvenido, "+from_name+".\n";
			welcome += "Usa los siguientes comandos para controlar el LED\n";
			welcome += "/led_on para encender el GPIO\n";
			welcome += "/led_off para encender el GPIO\n";
			welcome += "/state para solicitar el estado actual del GPIO";
			bot.sendMessage(chat_id, welcome, "");
		}
		if (text=="/led_on"){
			bot.sendMessage(chat_id, "LED encendido", "");
			ledState = HIGH;
			digitalWrite(ledPin, ledState);
		}
		if (text=="/led_off"){
			bot.sendMessage(chat_id, "LED apagado", "");
			ledState = LOW;
			digitalWrite(ledPin, ledState);
		}
		if (text=="/state"){
			if(digitalRead(ledPin)){
				bot.sendMessage(chat_id, "Estado del LED es: Encendido", ""); 
			}else{
				bot.sendMessage(chat_id, "Estado del LED es: Apagado", "");
			}
		}
	}
}

void setup() {
	Serial.begin(115200);
  
	#ifdef ESP8266
		configTime(0, 0, "pool.ntp.org");
		client.setTrustAnchors(&cert);
	#endif
	
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, ledState);
	
	//Conectar Wi-Fi
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	#ifdef ESP32
		client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
	#endif
	while(WiFi.status() != WL_CONNECTED){
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}
	Serial.println(WiFi.localIP()); 
}

void loop() {
	if (millis()>lastTimeBotRan+botRequestDelay){
		int numNewMessages = bot.getUpdates(bot.last_message_received+1);
		
		while(numNewMessages){
			Serial.println("got response");
			handleNewMessages(numNewMessages);
			numNewMessages=bot.getUpdates(bot.last_message_received+1);
		}
		lastTimeBotRan=millis();
	}
}

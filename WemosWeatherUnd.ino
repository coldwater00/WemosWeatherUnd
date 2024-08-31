
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include <WEMOS_SHT3X.h>
#include "properties.h"

SHT3X sht30(SHT30_ADDRESS);

uint SLEEP_TYPE = 3;  //0=no sleep; 1=//model sleep ;2=// Light sleep ( 4 ma ); 3 = //deep sleep

void setup() {
  Serial.begin(19200);
  
  Serial.println("Initializing SHT30 sensor...");
  
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("\nWiFi connected");
  setupwifi();
}

void setupwifi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi.disconnect");
    WiFi.disconnect();
  }
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_OFF);  //Prevents reconnection issue (taking too long to connect)
  Serial.println("WIFI_OFF");
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.println("WIFI_STA");
  Serial.println(String("") + "wifi mode:" + WiFi.getMode());
}


void connectToWifi() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    //digitalWrite(2, LOW);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int n = 0;
    while (WiFi.status() != WL_CONNECTED) {
      //WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(1000);
      if (n++ > 300) {
        ESP.restart();
      }
    }
    delay(5000);
    Serial.println("\nConnected.");
    //digitalWrite(2, HIGH);
  }
}

void loop() {
  connectToWifi();
  if(sht30.get() == 0){
    float temperature = sht30.cTemp;

    float humidity = sht30.humidity;

    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
    temperature=temperature*CORRECTION;
    Serial.printf("Mod. Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
    //sendToWunderground(temperature, humidity);
    sendToWeatherUnderground(temperature, humidity);
  } else {
    Serial.println("Failed to read from SHT30 sensor!");
  }

  // Deep sleep
  Serial.printf("Going to deep sleep for %d seconds...\n", SLEEP_DURATION);
  //ESP.deepSleep(SLEEP_DURATION * 1e6); // Converti secondi in microsecondi
  //delay(5000);
  sleep(SLEEP_DURATION);
  wakeUp();
}


// Funzione per inviare i dati a Weather Underground
void sendToWeatherUnderground(float temperature, float humidity) {
    String wuServer = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?";
    String wuID = String(WU_STATION_ID);
    String wuPassword = String(WU_API_KEY) ;
    String wuURL = wuServer + "ID=" + wuID + "&PASSWORD=" + wuPassword +
                   "&dateutc=now&tempf=" + String(temperature * 1.8 + 32) + // Convertire in Fahrenheit
                   "&humidity=" + String(humidity) +
                   "&action=updateraw";

    WiFiClientSecure client;
    client.setInsecure();  // Disabilita la verifica del certificato (opzionale, per server senza certificato affidabile)
    HTTPClient https;

    if (https.begin(client, wuURL)) {  // Inizializza la connessione HTTPS
        int httpCode = https.GET();
        if (httpCode > 0) {
            Serial.printf("HTTP GET Status Code: %d\n", httpCode);
        } else {
            Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
    } else {
        Serial.println("Impossibile iniziare la connessione HTTPS");
    }
}



void callback() {
  Serial1.println("Callback");
  Serial.flush();
}

//sleep and wait
void sleep(uint32_t sleep_time) {

  Serial.println(String()+"Slepping for "+sleep_time+" seconds...");
  Serial.flush();
  delay(100);

  switch (SLEEP_TYPE) {
    case 0:  // no sleep
      // statements
      Serial.println("no sleep");
      delay(1000 * sleep_time);
      break;
    case 1:  //model sleep
      //model sleep ( 18 ma )
      Serial.println("model sleep");
      WiFi.forceSleepBegin();
      delay(1);
      delay(1000 * sleep_time);
      break;
    case 2:  // Light sleep ( 4 ma )
      Serial.println("Light sleep");
      WiFi.disconnect();
      // wifi_set_opmode(NULL_MODE);
      // wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
      // wifi_fpm_open();
      // wifi_fpm_set_wakeup_cb(callback);
      // for (int i = 0; i < sleep_time; i++) {
      //   wifi_fpm_do_sleep(10000);
      //   delay(1000 + 1);
      // }

      wifi_set_opmode(NULL_MODE);
      wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
      wifi_fpm_open();
      wifi_fpm_set_wakeup_cb(callback);
      wifi_fpm_do_sleep(sleep_time*1000 *1000 );
      delay(sleep_time*1000 + 1);

      break;
    case 3:  //deep sleep + HW connection
      Serial.println("deep sleep");
      //Deep Sleep ( ) + reset cable
      ESP.deepSleep(sleep_time * 1000000);
      break;
    default:
      delay(1000 * sleep_time);
      break;
  }
 
  Serial.println("SleppeD....");
}

void wakeUp() {
  int num=0;
  int max=10;
  Serial.println("wakeUp");
                //---
  switch (SLEEP_TYPE) {
    case 0:  // no sleep
      // statements
      break;
    case 1:  //model sleep
      WiFi.forceSleepWake();
      delay(100);
      while (!WiFi.isConnected()) {
        Serial.println("Wait connection....");
        delay(2000);
        if(num++>max)
          ESP.restart();
      }
      break;
    case 2:  // Light sleep ( 4 ma )
             //light sleep
      wifi_set_sleep_type(NONE_SLEEP_T);
      delay(10);
      connectToWifi();
      Serial.println(String("") + "wifi mode:" + WiFi.getMode());
      break;
    case 3:  //deep sleep
      //Deep Sleep ( ) + reset cable
      //nothing to do
      break;
  }
  delay(2000);  // sure next sensors read are ok
}
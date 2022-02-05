#include <Arduino.h>
#include "Wifi.h" 

void WifiConnect(const char *name, const char *password){
    // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(name);
  WiFi.hostname("Wemos");
  WiFi.begin(name, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

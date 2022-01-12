#include <Arduino.h>
#include "Wifi.h" 

void WifiConnect(const char *a,const char *b){
    // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(a);
  WiFi.hostname("Wemos");
  WiFi.begin(a, b);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

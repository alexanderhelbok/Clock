#include "Wifi.h" 

void WifiConnect(const char *name, const char *password){
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepWake();
  delay(1);     //For some reason the modem won't go to sleep unless you do a delay(non-zero-number)

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

void WifiDisconnect(){
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1); //For some reason the modem won't go to sleep unless you do a delay
}

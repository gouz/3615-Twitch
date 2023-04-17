#include "Minitel1B_Soft.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

Minitel minitel(3, 1);  // RX, TX

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

AsyncWebServer server(80);

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  for(size_t i = 0; i < len; i++){
    minitel.writeByte(data[i]);
  }
  request->send(200);
}

void setup() {  
  minitel.changeSpeed(minitel.searchSpeed());
  minitel.newScreen();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    minitel.newScreen();
    minitel.println("Connecting to WiFi...");
    delay(1000);
  }
  minitel.newScreen();
  minitel.println(WiFi.localIP().toString());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello world!");
  });

  server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.newScreen();
    request->send(200);
  });

  server.on("/end", HTTP_GET, [](AsyncWebServerRequest *request) {
    minitel.noCursor();
    request->send(200);
  });

   server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {}, handleUpload);

  // accept CORS
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*"); 
  // Start server
  server.begin();

  minitel.println("Ready!");
}

void loop() {}
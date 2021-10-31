#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const int LED_PORT = 23;
const int SENSOR_PORT = 13;
const char* ssid = "HOME-1";
const char* password = "5up3rM4r10W0rld";
String ledState;
String buttonState;
bool isOpened = false; 
AsyncWebServer server(80);

void triggerLedOn();
void triggerLedOff();
void blink();


String garageStateProcessor(const String& state) {
  Serial.println(state);

  if (state == "STATE") {
    if (isOpened) {
      ledState = "OPEN";
    } else {
      ledState = "CLOSED";
    }

    Serial.print(ledState);
    return ledState;
  }

  return String();
}

String buttonProcessor(const String& state) {
  if (state == "BUTTON_STATE") {
    if (isOpened) {
      buttonState = "CLOSE";
    } else {
      buttonState = "OPEN";
    }

    return buttonState;
  }

  return String();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PORT, OUTPUT);
  pinMode(SENSOR_PORT, INPUT);

  
  // Init SPIFFS (file system)
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print local IP 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  // HTTP for webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request -> send(SPIFFS, "/index.html", String(), false, garageStateProcessor);
    // request -> send(SPIFFS, "/index.html", String(), false, buttonProcessor);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request -> send(SPIFFS, "/style.css", "text/css");
  });

  // on and off routes
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    // isOpened = true;
    digitalWrite(LED_PORT, HIGH);
    delay(1000);
    digitalWrite(LED_PORT, LOW);
    request -> send(SPIFFS, "/index.html", String(), false, garageStateProcessor);
    // request -> send(SPIFFS, "/index.html", String(), false, buttonProcessor);
  });

  /*
  server.on("/close", HTTP_GET, [](AsyncWebServerRequest *request){
    // isOpened = false;
    digitalWrite(LED_PORT, HIGH);
    request -> send(SPIFFS, "/index.html", String(), false, processor);
    delay(500);
    digitalWrite(LED_PORT, LOW);
  });
  */

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    // isOpened = false;
    request -> send(SPIFFS, "/index.html", String(), false, garageStateProcessor);
    // request -> send(SPIFFS, "/index.html", String(), false, buttonProcessor);
  });
  
  server.begin();

 
}

void loop() {
  int valSensor = 0;

  valSensor = digitalRead(SENSOR_PORT);   // read the input pin
  Serial.println(valSensor);
  
  if (valSensor == HIGH) {
    isOpened = false;
  } else if (valSensor == LOW) {
    isOpened = true;
  }

  garageStateProcessor;
  buttonProcessor;
  delay(1500);
}


void triggerLedOn() {
  digitalWrite(LED_PORT, HIGH);  
  Serial.println("Connected");  
}

void triggerLedOff() {
  digitalWrite(LED_PORT, LOW);  
  Serial.println("Disonnected");  
}

void blink() {
  digitalWrite(LED_PORT, HIGH);
  delay(500);
  digitalWrite(LED_PORT, LOW);
  delay(500);
  digitalWrite(LED_PORT, HIGH);
  delay(500);
  digitalWrite(LED_PORT, LOW);
  delay(500);
}


/*

  // Includes
  #include <Arduino.h>
  #include <WiFi.h>

  // Network Credentials
  const char* ssid = "HOME-1";
  const char* password = "5up3rM4r10W0rld";

  // Setting web server port
  WifiServer server(8080);

  // HTTP header

*/

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const int LED_PORT = 23;
const int SENSOR_PORT = 13;
const char* ssid = "HOME-1";
const char* password = "5up3rM4r10W0rld";
AsyncWebServer server(80);

void triggerLedOn();
void triggerLedOff();
void blink();

void setup() {
  pinMode(LED_PORT, OUTPUT);
  pinMode(SENSOR_PORT, INPUT);
  // Serial.begin(9600);
  blink();

  Serial.begin(9600);  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {  
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

 Serial.println("Connected to the WiFi network");
 Serial.println(WiFi.localIP());

 server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
  request -> send(200, "text/plain", "Hello world");
  triggerLedOn();
  });

  server.begin();
}

void loop() {
  int valSensor = 0;

  valSensor = digitalRead(SENSOR_PORT);   // read the input pin
  Serial.println(valSensor);
  
  if (valSensor == HIGH) {
    triggerLedOn();
  } else if (valSensor == LOW) {
    triggerLedOff();
  } 

  /* 
  if (Serial.available() > 0) {
    char incomingSerial = Serial.read();
    Serial.println(incomingSerial);
    if (incomingSerial == 'Y' || incomingSerial == 'y') {
      blink();
      Serial.println("here1");
    } else if (incomingSerial == 'N' || incomingSerial == 'n') {
      digitalWrite(LED_PORT, LOW);
      Serial.println("here2");
    }
  }
  */
  
  delay(500);
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

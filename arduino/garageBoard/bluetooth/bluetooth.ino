#include "BluetoothSerial.h";
BluetoothSerial SerialBT;

void setup(){
  Serial.begin(9600);
  SerialBT.begin("My ESP32");
}
void loop(){
 if (SerialBT.hasClient()) {
     SerialBT.println("Hello");
     Serial.println("hello");
  }
  delay(200);
}

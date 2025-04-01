#include <WiFi.h>

const char* ssid = "ESP32_Network";
const char* password = "123456789";
WiFiServer server(80);

// Default motor speed (0-255)
int motorSpeed = 70;

void forward() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, 0);
}

void backward() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, motorSpeed);
}

void right() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, 0);
}

void left() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, motorSpeed);
}

void stop() {
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
}

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  Serial.begin(115200);
  
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String data = client.readStringUntil('\n');
        Serial.println("Received: " + data);
        
        // Direction commands
        if (data == "up\r") {
          forward();
        }
        else if (data == "down\r") {
          backward();
        }
        else if (data == "left\r") {
          left();
        }
        else if (data == "right\r") {
          right();
        }
        else if (data == "PRESSED\r") {
          stop();
        }
      }
    }
    client.stop();
  }
}

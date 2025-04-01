#include <WiFi.h>

const char* ssid = "ESP32_Network";
const char* password = "123456789";
const char* serverIP = "192.168.4.1"; // Default AP IP
const int serverPort = 80;

void setup() {
  Serial.begin(115200);
  
  // Connect to server ESP32
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to ESP32 Server");
}

void loop() {
  WiFiClient client;
  
  if (client.connect(serverIP, serverPort)) {
    // Send data
    client.println("Hello from ESP32 Client!");
    
    // Wait for response
    while (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println("Server response: " + response);
    }
    
    client.stop();
  }
  
  delay(5000); // Wait 5 seconds before next transmission
}

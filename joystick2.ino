#include <WiFi.h>

const char* ssid = "ESP32_Network";
const char* password = "123456789";
const char* serverIP = "192.168.4.1"; 
const int serverPort = 80;
int buttonState = 0;     
int clearState = 0;
int playbackState = 0; 

#define VRX_PIN  39 
#define VRY_PIN  36 
#define BUTTON_PIN 34
#define CLEAR_PIN 35
#define PLAYBACK_BUTTON 32

int valueX = 0;
int valueY = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(CLEAR_PIN, INPUT);
  pinMode(PLAYBACK_BUTTON, INPUT);
  Serial.begin(115200);

  analogSetAttenuation(ADC_11db);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi Server");
  Serial.print("Client IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);
  buttonState = digitalRead(BUTTON_PIN);
  clearState = digitalRead(CLEAR_PIN);
  playbackState = digitalRead(PLAYBACK_BUTTON);

  String direction = "";

  if (valueY == 4095) {
    direction = "up";
  } else if (valueY == 0) {
    direction = "down";
  } if (valueX == 0) {
    direction = "left";
  } else if (valueX == 4095) {
    direction = "right";
  }
  else if(buttonState == HIGH){
    direction = "PRESSED";
  }
  else if(clearState == HIGH){
    direction = "clear";
  }
  else if(playbackState == HIGH){
    direction = "playback";
  }
  Serial.println(direction);
  if (!direction.isEmpty()) {
    WiFiClient client;

    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to server");

      client.println(direction);
      Serial.println("Sent to server: " + direction);

      // while (client.available()) {
      //   String response = client.readStringUntil('\n');
      //   Serial.println("Server response: " + response);
      // }

      // client.stop();
    } else {
      Serial.println("Failed to connect to server");
    }
  }

  delay(200);
}


#include <WiFi.h>
#include <SPIFFS.h>

const char* ssid = "ESP32_Network";
const char* password = "123456789";
WiFiServer server(80);

// Default motor speed (0-255)
int motorSpeed = 90;

// Movement recording variables
unsigned long movementStartTime = 0;
String currentMovement = "";
File commandFile;

void printCommandFile() {
  // Open the commands file to read
  File commandFile = SPIFFS.open("/commands.txt", "r");
  if (!commandFile) {
    Serial.println("Failed to open commands file for reading");
    return;
  }
  
  Serial.println("--- Command File Contents ---");
  while (commandFile.available()) {
    String line = commandFile.readStringUntil('\n');
    Serial.println(line);
  }
  Serial.println("--- End of File ---");
  
  commandFile.close();
}

void forward() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, 0);
  
  // Start timing movement
  movementStartTime = millis();
  currentMovement = "forward";
}

void backward() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, motorSpeed);
  
  // Start timing movement
  movementStartTime = millis();
  currentMovement = "backward";
}

void right() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, 0);
  
  currentMovement = "right";
}

void left() {
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, motorSpeed);
  
  currentMovement = "left";
}

void stop() {
  // Calculate movement duration if applicable
  if (!currentMovement.isEmpty() && 
      (currentMovement == "forward" || currentMovement == "backward")) {
    unsigned long movementDuration = millis() - movementStartTime;
    
    // Open file in append mode
    File commandFile = SPIFFS.open("/commands.txt", "a");
    if (commandFile) {
      commandFile.println(currentMovement + " " + String(movementDuration));
      commandFile.close();
      
      // Print file contents after saving
      printCommandFile();
    }
  }
  else if (!currentMovement.isEmpty()) {
    // For left and right, just record the movement
    File commandFile = SPIFFS.open("/commands.txt", "a");
    if (commandFile) {
      commandFile.println(currentMovement);
      commandFile.close();
      
      // Print file contents after saving
      printCommandFile();
    }
  }
  
  // Reset movement tracking
  currentMovement = "";
  movementStartTime = 0;
  
  // Stop motors
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
}

void playbackCommands() {
  // Open and read the commands file
  File commandFile = SPIFFS.open("/commands.txt", "r");
  if (!commandFile) {
    Serial.println("Failed to open commands file");
    return;
  }
  
  while (commandFile.available()) {
    String command = commandFile.readStringUntil('\n');
    Serial.println("Replaying: " + command);
    
    if (command.startsWith("forward")) {
      forward();
      int duration = command.substring(8).toInt();
      delay(duration);
      stop();
    }
    else if (command.startsWith("backward")) {
      backward();
      int duration = command.substring(9).toInt();
      delay(duration);
      stop();
    }
    else if (command == "left") {
      left();
      delay(2000);
      stop();
    }
    else if (command == "right") {
      right();
      delay(2000);
      stop();
    }
  }
  
  commandFile.close();
}

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
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
          delay(2000);
          stop();
        }
        else if (data == "right\r") {
          right();
          delay(2000);
          stop();
        }
        else if (data == "PRESSED\r") {
          stop();
        }
        // New command to playback recorded commands
        else if (data == "playback\r") {
          playbackCommands();
        }
        // New command to clear recorded commands
        else if (data == "clear\r") {
          SPIFFS.remove("/commands.txt");
          Serial.println("Commands file cleared");
        }
      }
    }
    client.stop();
  }
}
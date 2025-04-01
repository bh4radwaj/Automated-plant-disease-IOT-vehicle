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
bool isRecording = false;
File commandFile;

// Continuous playback variables
bool continuousPlayback = false;

void logPreviousCommandWithTime() {
  if (isRecording && !currentMovement.isEmpty()) {
    unsigned long duration = millis() - movementStartTime;
    
    // Open file in append mode
    File file = SPIFFS.open("/commands.txt", "a");
    if (file) {
      // For left and right, don't log duration
      if (currentMovement == "left" || currentMovement == "right") {
        file.println(currentMovement);
      } else {
        // For forward and backward, log with duration
        file.println(currentMovement + " " + String(duration));
      }
      file.close();
      //Serial.println("Logged: " + currentMovement + (currentMovement == "left" || currentMovement == "right" ? "" : " " + String(duration)));
      // printRecordedCommands();
    }
  }
}

void startNewCommand(String command) {
  // Log the previous command with its time first
  logPreviousCommandWithTime();
  
  // Set up for the new command
  currentMovement = command;
  movementStartTime = millis();
}

void forward() {
  motorSpeed = 40;
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, 0);
  delay(200);
  motorSpeed = 20;
  analogWrite(12, motorSpeed);
  analogWrite(27, motorSpeed);
  
  // If this is the first command, start recording
  if (!isRecording) {
    isRecording = true;
    // Delete existing commands file to start fresh
    SPIFFS.remove("/commands.txt");
    //Serial.println("Recording started");
  }
  
  // Start timing new movement
  startNewCommand("forward");
}

void backward() {
  motorSpeed = 40;
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, motorSpeed);
  delay(200);
  motorSpeed = 20;
  analogWrite(26, motorSpeed);
  analogWrite(14, motorSpeed);
  
  if (isRecording) {
    // Start timing new movement
    startNewCommand("backward");
  }
}

void right() {
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  delay(500);
  motorSpeed = 85;
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, 0);
  analogWrite(27, motorSpeed);
  analogWrite(26, 0);
  analogWrite(14, motorSpeed);
  
  delay(400);

  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  
  if (isRecording) {
    // Start timing new movement
    startNewCommand("right");
    
    // Fixed delay for turn
    delay(2000);
    stop();
  } else {
    // If not recording, just do the turn with delay
    delay(2000);
    stop();
  }
}

void left() {
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  delay(500);
  motorSpeed = 85;
  digitalWrite(13, HIGH);
  digitalWrite(25, HIGH);
  analogWrite(12, motorSpeed);
  analogWrite(27, 0);
  analogWrite(26, motorSpeed);
  analogWrite(14, 0);
    
  delay(400);

  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  
  if (isRecording) {
    // Start timing new movement
    startNewCommand("left");
    
    // Fixed delay for turn
    delay(2000);
    stop();
  } else {
    // If not recording, just do the turn with delay
    delay(2000);
    stop();
  }
}

void stop() {
  // Stop motors
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
}

void finishRecording() {
  if (isRecording) {
    // Log the final command with its time
    logPreviousCommandWithTime();
    
    isRecording = false;
    currentMovement = "";
    movementStartTime = 0;
    
    //Serial.println("Recording stopped");
    
    // Print the recorded commands to serial monitor
    printRecordedCommands();
  }
}

void printRecordedCommands() {
  //Serial.println("Recorded commands:");
  File commandFile = SPIFFS.open("/commands.txt", "r");
  if (commandFile) {
    Serial.println("AAAAAAAAAAAAAAAAAAAA");
    while (commandFile.available()) {
      String line = commandFile.readStringUntil('\n');
      Serial.println(line);
    }
    commandFile.close();
  } else {
    // Serial.println("Failed to open commands file for reading");
  }
}

// New function to store commands in memory
void loadCommands(String commands[], int &commandCount) {
  File commandFile = SPIFFS.open("/commands.txt", "r");
  commandCount = 0;
  
  if (commandFile) {
    while (commandFile.available() && commandCount < 100) { // Limit to 100 commands for safety
      commands[commandCount] = commandFile.readStringUntil('\n');
      commands[commandCount].trim();
      commandCount++;
    }
    commandFile.close();
  }
}

// Function to execute a single command
void executeCommand(String command) {
  //Serial.println("Executing: " + command);
  
  if (command.startsWith("forward")) {
    forward();
    // If there's a duration specified
    if (command.indexOf(' ') > 0) {
      int duration = command.substring(command.indexOf(' ') + 1).toInt();
      delay(duration);
    } else {
      delay(1000);  // Default time if not specified
    }
    stop();
  }
  else if (command.startsWith("backward")) {
    backward();
    // If there's a duration specified
    if (command.indexOf(' ') > 0) {
      int duration = command.substring(command.indexOf(' ') + 1).toInt();
      delay(duration);
    } else {
      delay(1000);  // Default time if not specified
    }
    stop();
  }
  else if (command == "left") {
    left();
    // The left() function already has delay and stop inside
  }
  else if (command == "right") {
    right();
    // The right() function already has delay and stop inside
  }
}

// Function to reverse a command
String reverseCommand(String command) {
  if (command.startsWith("forward")) {
    int duration = 1000;
    if (command.indexOf(' ') > 0) {
      duration = command.substring(command.indexOf(' ') + 1).toInt();
    }
    return "backward " + String(duration);
  }
  else if (command.startsWith("backward")) {
    int duration = 1000;
    if (command.indexOf(' ') > 0) {
      duration = command.substring(command.indexOf(' ') + 1).toInt();
    }
    return "forward " + String(duration);
  }
  else if (command == "left") {
    return "right";
  }
  else if (command == "right") {
    return "left";
  }
  return command; // In case of unknown command
}

void playbackCommandsWithReversal() {
  String commands[100]; // Assuming max 100 commands
  int commandCount = 0;
  
  // Load all commands into memory
  loadCommands(commands, commandCount);
  
  if (commandCount == 0) {
    //Serial.println("No commands to play back");
    return;
  }
  
  //Serial.println("Starting continuous playback loop");
  continuousPlayback = true;
  
  while (continuousPlayback) {
    // Forward playback
    //Serial.println("--- Forward Playback ---");
    for (int i = 0; i < commandCount; i++) {
      executeCommand(commands[i]);
      
      // Check if playback was stopped
      if (!continuousPlayback) break;
    }
    
    // 2-second delay between forward and reverse
    if (continuousPlayback) {
      //Serial.println("Pausing for 2 seconds before reversing");
      delay(2000);
    }
    
    // Reverse playback
    if (continuousPlayback) {
      //Serial.println("--- Reverse Playback ---");
      for (int i = commandCount - 1; i >= 0; i--) {
        String reversedCommand = reverseCommand(commands[i]);
        executeCommand(reversedCommand);
        
        // Check if playback was stopped
        if (!continuousPlayback) break;
      }
    }
    
    // 2-second delay before starting again
    if (continuousPlayback) {
      //Serial.println("Pausing for 2 seconds before repeating");
      delay(2000);
    }
  }
  
  //Serial.println("Continuous playback stopped");
}

void stopPlayback() {
  continuousPlayback = false;
  stop();
  //Serial.println("Stopping continuous playback");
}

void playbackCommands() {
  // We'll use the new playback function
  playbackCommandsWithReversal();
}

void clearCommands() {
  if (SPIFFS.remove("/commands.txt")) {
    //Serial.println("Commands file completely deleted");
  } else {
    //Serial.println("Failed to delete commands file");
  }
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
    //Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  WiFi.softAP(ssid, password);
  //Serial.println("Access Point Started");
  //Serial.print("IP Address: ");
  //Serial.println(WiFi.softAPIP());
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String data = client.readStringUntil('\n');
        //Serial.println("Received: " + data);
        
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
          finishRecording();  // Stop recording and print commands
        }
        // Playback recorded commands
        else if (data == "playback\r") {
          playbackCommands();
        }
        // Stop playback
        else if (data == "stop\r") {
          stopPlayback();
        }
        // Clear recorded commands
        else if (data == "clear\r") {
          clearCommands();
        }
      }
    }
    client.stop();
  }
}
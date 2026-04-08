// --- ADVANCED AUDIO SETTINGS (Must be at the top) ---
#define MAX_RECORD_TIME_MS 6000      
#define COMPRESSION_TARGET_LEVEL 125 
#include <WiFi.h>
#include <ESP32AI.h>
#include <ArduinoJson.h>

// --- USER CONFIGURATION ---
const char* ssid =     "VRV9517C4C8EC";
const char* pass =     "CVXLGAp6eRxY";
const char* endpoint = "https://dark-base-249d.546682.workers.dev";

// --- PIN DEFINITIONS ---
#define BTN_PIN 23    
#define BUZZER  13    
#define MIC_SCK 32
#define MIC_WS  25
#define MIC_SD  33

// --- MUSICAL NOTES FOR HAPPY BIRTHDAY ---
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523

int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

int durations[] = {
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 4, 4,
  8, 8, 4, 4, 4, 2
};

ESP32AI ai;

// --- SKILLS DEFINITION ---
const char* mySkills = R"([
  {
    "Target": "Greeting",
    "Options": ["SayHello"],
    "Actions": [
      { "Name": "SayHello", "Type": "boolean", "Variants": ["hello", "hi", "hey"] }
    ]
  },
  {
    "Target": "Radio",
    "Options": ["Play"],
    "Actions": [
      { "Name": "Play", "Type": "boolean", "Variants": ["play music", "start radio", "sing a song", "happy birthday"] }
    ]
  }
])";

// Function to play the melody
void playHappyBirthday() {
  for (int i = 0; i < 25; i++) {
    int duration = 1000 / durations[i];
    tone(BUZZER, melody[i], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n[SUCCESS] WiFi Connected");

  if (!ai.begin()) {
    Serial.println("[ERROR] AI Library failed!");
    return;
  }
  
  ai.setWorkerAIEndpoint(endpoint);
  ai.setSkills(mySkills);
  ai.configureI2S(MIC_SCK, MIC_WS, MIC_SD);
  ai.configureRecordingPin(BTN_PIN, true);
  
  // Startup beep
  tone(BUZZER, NOTE_C5, 100); delay(150); tone(BUZZER, NOTE_E4, 100);
  
  Serial.println("--- SYSTEM READY ---");
  Serial.println("Try saying: 'Play music' or 'Happy Birthday'");
}

void loop() {
  if (ai.startListening()) {
    tone(BUZZER, NOTE_G4, 50); // Beep when recording starts
    Serial.println("Recording...");
    
    SkillResponse response = ai.processCommand();
    
    if (response.hasAction) {
      Serial.printf("AI Action matched: %s\n", response.actionName.c_str());

      if (response.actionName == "SayHello") {
        Serial.println("Action: Greeting");
        tone(BUZZER, NOTE_C5, 100); delay(100); tone(BUZZER, NOTE_C5, 100);
      } 
      else if (response.actionName == "Play") {
        Serial.println("Action: Playing Happy Birthday!");
        playHappyBirthday();
      }
    } 
    else {
      Serial.println("AI could not understand.");
      tone(BUZZER, 150, 500); // Low error tone
    }
  }

  // Serial commands
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'C' || c == 'c') {
      Serial.println("Calibrating...");
      ai.calibrateSilenceThreshold();
    }
  }
}
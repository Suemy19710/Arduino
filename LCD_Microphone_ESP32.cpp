#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22

#define MIC_PIN 34
#define BUZZER_PIN 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long lastTriggerTime = 0;
const unsigned long triggerCooldown = 2000;   

// ---------- Read microphone amplitude ----------
int readAmplitude() {
  int signalMax = 0;
  int signalMin = 4095;

  for (int i = 0; i < 100; i++) {
    int sample = analogRead(MIC_PIN);

    if (sample > signalMax) signalMax = sample;
    if (sample < signalMin) signalMin = sample;
  }

  return signalMax - signalMin;
}

void showScreen(String line1, String line2 = "", String line3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println(line1);

  display.setCursor(0, 20);
  display.println(line2);

  display.setCursor(0, 40);
  display.println(line3);

  display.display();
}

// ---------- Passive buzzer tone ----------
void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration + 20);
  noTone(BUZZER_PIN);
}

// ---------- Melody when sound detected ----------
void playGreetingMelody() {
  playTone(523, 150);   // C5
  playTone(659, 150);   // E5
  playTone(784, 200);   // G5
}

// ---------- Louder response melody ----------
void playLoudMelody() {
  playTone(784, 120);   // G5
  playTone(880, 120);   // A5
  playTone(988, 180);   // B5
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true);
  }

  pinMode(BUZZER_PIN, OUTPUT);

  display.clearDisplay();
  display.display();

  showScreen("System Ready", "MAX9814 + OLED", "Listening...");
  delay(1500);
}

void loop() {
  int amplitude = readAmplitude();

  Serial.print("Amplitude: ");
  Serial.println(amplitude);

  // live display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("Listening...");

  display.setCursor(0, 18);
  display.print("Sound level: ");
  display.println(amplitude);

  int barWidth = map(amplitude, 0, 1500, 0, 120);
  if (barWidth < 0) barWidth = 0;
  if (barWidth > 120) barWidth = 120;

  display.drawRect(4, 40, 120, 12, WHITE);
  display.fillRect(4, 40, barWidth, 12, WHITE);

  display.display();

  // trigger logic with cooldown
  if (millis() - lastTriggerTime > triggerCooldown) {

    if (amplitude > 900) {
      showScreen("Hello!", "I hear you", "Loud input");
      playLoudMelody();
      lastTriggerTime = millis();
    }
    else if (amplitude > 450) {
      showScreen("Hi!", "Sound detected", "Nice to meet you");
      playGreetingMelody();
      lastTriggerTime = millis();
    }
  }

  delay(100);
}
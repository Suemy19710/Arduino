#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

String cmd = "";

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1) {}
  }

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  roboEyes.setMood(DEFAULT);
}

void loop() {
  roboEyes.update();

  // Read one line command like "HAPPY"
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      cmd.trim();
      cmd.toUpperCase();

      if (cmd == "HAPPY") roboEyes.setMood(HAPPY);
      else if (cmd == "ANGRY") roboEyes.setMood(ANGRY);
      else if (cmd == "TIRED") roboEyes.setMood(TIRED);
      else if (cmd == "DEFAULT") roboEyes.setMood(DEFAULT);

      cmd = "";
    } else {
      cmd += c;
    }
  }
}
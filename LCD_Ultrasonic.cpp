#include <LiquidCrystal.h>

// const int buzzer = 9;
const int LED1 = 11;
const int LED2 = 10;
const int LED3 = 9;

const int echo = 13;
const int trig = 12;

LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

// unsigned long duration = 0;
int duration = 0;
float distance = 0;

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // digitalWrite(trig, LOW);

  Serial.begin(9600);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Ultrasonic Ready");
  delay(800);
  lcd.clear();
}

void loop() {
  digitalWrite(trig , HIGH);
  delayMicroseconds(1000);
  digitalWrite(trig , LOW);

  // duration = pulseIn(echo, HIGH, 30000);

  duration = pulseIn(echo , HIGH);

  if (duration == 0) {
    Serial.println("No echo");
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

    lcd.setCursor(0, 0);
    lcd.print("No echo        ");
    delay(100);
    return;
  }

  distance = (duration/2) / 28.5 ;
  Serial.println(distance);

  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance, 1);
  lcd.print(" cm   ");

  if (distance <= 27) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }
  else if (distance <= 37) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
  }
  else if (distance <= 47) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
  }
  else {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);

  }

  delay(60);
}
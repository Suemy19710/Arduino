// ================= Arduino Library =================
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <string.h>

// ================= LCD ================
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ================= SERVO =================
Servo servo;
const int SERVO_PIN  = 7;    
const int LOCK_POS   = 0;     // locked angle
const int OPEN_POS   = 180;    // open angle

// ================= KEYPAD 4x4 =================
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 10, 9, 8};
byte colPins[COLS] = {A0, A1, A2, A3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ================= PASSWORD =================
const char correctPass[] = "1234";
char inputPass[5];   // 4 digits + '\0'
byte idx = 0;

void lockDoor() {
  servo.write(LOCK_POS);
}

void openDoor() {
  servo.write(OPEN_POS);
}

void resetScreen() {
  idx = 0;
  inputPass[0] = '\0';
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Pass:");
  lcd.setCursor(0, 1);
}

void setup() {
  lcd.begin(16, 2);

  servo.attach(SERVO_PIN);
  lockDoor();

  resetScreen();
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  // CLEAR
  if (key == '*') {
    lockDoor();
    resetScreen();
    return;
  }

  // ENTER
  if (key == '#') {
    inputPass[idx] = '\0';

    if (strcmp(inputPass, correctPass) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password OK");
      lcd.setCursor(0, 1);
      lcd.print("Door Opening");

      openDoor();          // servo runs (opens)
      delay(3000);         // keep open 3 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Door Closing");

      lockDoor();          // lock again
      delay(1200);

    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password");
      lcd.setCursor(0, 1);
      lcd.print("Try Again");
      lockDoor();
      delay(1500);
    }

    resetScreen();
    return;
  }

  // Accept only digits, max 4
  if (key >= '0' && key <= '9') {
    if (idx < 4) {
      inputPass[idx++] = key;
      lcd.print('*');      // hide digits
    }
  }
}
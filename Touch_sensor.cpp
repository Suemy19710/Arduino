const int ctsPin = 2;
const int ledPin = 11;
const int buzzer = 10;

// ----- Notes -----
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_C5 523

// Melody (25 notes)
const int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

// Durations (4 = quarter, 8 = eighth, 2 = half, 1 = whole)
const int noteDurations[] = {
  4,4,2,2,2,1,
  4,4,2,2,2,1,
  4,4,2,2,2,2,1,
  4,4,2,2,2,1
};

const int songLength = sizeof(melody) / sizeof(melody[0]);

// ---- Player state ----
int noteIndex = 0;
bool isPlaying = false;

unsigned long noteStartMs = 0;
unsigned long noteTotalMs = 0;

void startSong() {
  isPlaying = true;
  noteIndex = 0;
  noteStartMs = 0;      // force immediate start
  noteTotalMs = 0;
}

void stopSong() {
  isPlaying = false;
  noteIndex = 0;
  noTone(buzzer);
}

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // safer for touch/button style inputs:
  pinMode(ctsPin, INPUT_PULLUP);
}

void loop() {
  // With INPUT_PULLUP: touched/pressed usually reads LOW
  bool touched = (digitalRead(ctsPin) == LOW);

  if (!touched) {
    // Stop immediately when not touched
    digitalWrite(ledPin, LOW);
    if (isPlaying) {
      Serial.println("not touched -> STOP");
      stopSong();
    }
    return; // nothing else to do
  }

  // touched
  digitalWrite(ledPin, HIGH);

  if (!isPlaying) {
    Serial.println("TOUCHED -> start song");
    startSong();
  }

  // ---- Non-blocking player ----
  unsigned long now = millis();

  // If it's time to start the next note (or first note)
  if (noteStartMs == 0 || (now - noteStartMs) >= noteTotalMs) {

    // If finished the song, loop back to start (optional)
    if (noteIndex >= songLength) {
      noteIndex = 0;  // repeat while still touched
    }

    int durationMs = 1000 / noteDurations[noteIndex];

    // Note ON for 80% of duration, OFF for 20% (gives separation)
    int onTime = (durationMs * 80) / 100;
    int offTime = durationMs - onTime;

    // Start note
    tone(buzzer, melody[noteIndex], onTime);

    // Schedule total time for this note slot (on + off)
    noteStartMs = now;
    noteTotalMs = (unsigned long)onTime + (unsigned long)offTime;

    noteIndex++;
  }
}
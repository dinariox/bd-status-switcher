#define _NAME         "DiscordStatusSwitcher"
#define _VERSION      "0.0.1"
#define LED_GREEN     33
#define LED_YELLOW    32
#define LED_RED       13
#define LED_WHITE     12
#define BTN_GREEN     14
#define BTN_YELLOW    27
#define BTN_RED       26
#define BTN_WHITE     25
String serialInput;
String currentStatus;
bool buttonPressed;

void ledStartupAnimation() {
  const int ledPins[] = { LED_GREEN, LED_YELLOW, LED_RED, LED_WHITE };
  const int animDelay = 125;

  for (int i = 0; i < 4; i++) {
    if (i > 0) {
      digitalWrite(ledPins[i - 1], LOW);
    }
    digitalWrite(ledPins[i], HIGH);
    delay(animDelay);
  }

  for (int i = 2; i >= 0; i--) {
    digitalWrite(ledPins[i + 1], LOW);
    digitalWrite(ledPins[i], HIGH);
    delay(animDelay);
  }

  digitalWrite(ledPins[0], LOW);
  delay(animDelay);
}

void setSingleLed(String color) {
  // turn off all leds
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_WHITE, LOW);

  // turn on the selected led
  if (color == "green") {
    digitalWrite(LED_GREEN, HIGH);
  } else if (color == "yellow") {
    digitalWrite(LED_YELLOW, HIGH);
  } else if (color == "red") {
    digitalWrite(LED_RED, HIGH);
  } else if (color == "white") {
    digitalWrite(LED_WHITE, HIGH);
  }
}

void emitStatus() {
  Serial.println("status " + currentStatus);
}

void setup() {
  currentStatus = "unknown";
  buttonPressed = false;

  Serial.begin(38400);

  pinMode(LED_GREEN,    OUTPUT);
  pinMode(LED_YELLOW,   OUTPUT);
  pinMode(LED_RED,      OUTPUT);
  pinMode(LED_WHITE,    OUTPUT);
  pinMode(BTN_GREEN,    INPUT_PULLDOWN);
  pinMode(BTN_YELLOW,   INPUT_PULLDOWN);
  pinMode(BTN_RED,      INPUT_PULLDOWN);
  pinMode(BTN_WHITE,    INPUT_PULLDOWN);

  digitalWrite(LED_GREEN,   LOW);
  digitalWrite(LED_YELLOW,  LOW);
  digitalWrite(LED_RED,     LOW);
  digitalWrite(LED_WHITE,   LOW);

  delay(1000);
  ledStartupAnimation();
  ledStartupAnimation();
}

void loop() {
  if (Serial.available() > 0) {
    serialInput = Serial.readStringUntil('\n');

    if (serialInput.equals("name")) {
      Serial.write(_NAME);
    } else if (serialInput.equals("version")) {
      Serial.write(_VERSION);
    } else if (serialInput.equals("getstatus")) {
      Serial.println(currentStatus);
    } else if (serialInput.startsWith("setstatus")) {
      if (serialInput.endsWith("online")) {
        currentStatus = "online";
        setSingleLed("green");
      } else if (serialInput.endsWith("afk")) {
        currentStatus = "afk";
        setSingleLed("yellow");
      } else if (serialInput.endsWith("dnd")) {
        currentStatus = "dnd";
        setSingleLed("red");
      } else if (serialInput.endsWith("invisible")) {
        currentStatus = "invisible";
        setSingleLed("white");
      } else if (serialInput.endsWith("unknown")) {
        currentStatus = "unknown";
        setSingleLed(""); // all leds off
      }
    }
  }

  if (digitalRead(BTN_GREEN) == HIGH && !buttonPressed) {
    buttonPressed = true;
    currentStatus = "online";
    setSingleLed("green");
    emitStatus();
  } else if (digitalRead(BTN_YELLOW) == HIGH && !buttonPressed) {
    buttonPressed = true;
    currentStatus = "afk";
    setSingleLed("yellow");
    emitStatus();
  } else if (digitalRead(BTN_RED) == HIGH && !buttonPressed) {
    buttonPressed = true;
    currentStatus = "dnd";
    setSingleLed("red");
    emitStatus();
  } else if (digitalRead(BTN_WHITE) == HIGH && !buttonPressed) {
    buttonPressed = true;
    currentStatus = "invisible";
    setSingleLed("white");
    emitStatus();
  }

  if (!digitalRead(BTN_GREEN) && !digitalRead(BTN_YELLOW) && !digitalRead(BTN_RED) && !digitalRead(BTN_WHITE)) {
    buttonPressed = false;
  }
}

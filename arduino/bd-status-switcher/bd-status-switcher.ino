#define _NAME         "DiscordStatusSwitcher"
#define _VERSION      "0.0.1"
#define LED_BUILDIN   2
#define LED_GREEN     33
#define LED_YELLOW    32
#define LED_RED       13
#define LED_WHITE     12
#define BTN_GREEN     14
#define BTN_YELLOW    27
#define BTN_RED       26
#define BTN_WHITE     25
String serialInput;

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

void setup() {
  Serial.begin(38400);

  pinMode(LED_BUILDIN,  OUTPUT);
  pinMode(LED_GREEN,    OUTPUT);
  pinMode(LED_YELLOW,   OUTPUT);
  pinMode(LED_RED,      OUTPUT);
  pinMode(LED_WHITE,    OUTPUT);
  pinMode(BTN_GREEN,    INPUT_PULLDOWN);
  pinMode(BTN_YELLOW,   INPUT_PULLDOWN);
  pinMode(BTN_RED,      INPUT_PULLDOWN);
  pinMode(BTN_WHITE,    INPUT_PULLDOWN);
  
  digitalWrite(LED_BUILDIN, LOW);
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
    } else if (serialInput.startsWith("ledon")) {
      if (serialInput.endsWith("green")) {
        digitalWrite(LED_GREEN, HIGH);
        Serial.write("ledon green");
      } else if (serialInput.endsWith("yellow")) {
        digitalWrite(LED_YELLOW, HIGH);
        Serial.write("ledon yellow");
      } else if (serialInput.endsWith("red")) {
        digitalWrite(LED_RED, HIGH);
        Serial.write("ledon red");
      } else if (serialInput.endsWith("white")) {
        digitalWrite(LED_WHITE, HIGH);
        Serial.write("ledon white");
      }
    } else if (serialInput.startsWith("ledoff")) {
      if (serialInput.endsWith("green")) {
        digitalWrite(LED_GREEN, LOW);
        Serial.write("ledon green");
      } else if (serialInput.endsWith("yellow")) {
        digitalWrite(LED_YELLOW, LOW);
        Serial.write("ledon yellow");
      } else if (serialInput.endsWith("red")) {
        digitalWrite(LED_RED, LOW);
        Serial.write("ledon red");
      } else if (serialInput.endsWith("white")) {
        digitalWrite(LED_WHITE, LOW);
        Serial.write("ledon white");
      }
    }
  }
}

#define _NAME                   "DiscordStatusSwitcher"
#define _VERSION                "0.0.1"
#define CONNECTION_LOST_TIMEOUT 10000
// #define BAUDRATE                38400
#define BAUDRATE                500000

#define LED_GREEN               33
#define LED_YELLOW              32
#define LED_RED                 13
#define LED_WHITE               12
#define LED_MICROPHONE          5
#define LED_HEADPHONE           18

#define PWM_GREEN               0
#define PWM_YELLOW              1
#define PWM_RED                 2
#define PWM_WHITE               3
#define PWM_MICROPHONE          4
#define PWM_HEADPHONE           5

#define BTN_GREEN               14
#define BTN_YELLOW              27
#define BTN_RED                 26
#define BTN_WHITE               25
#define BTN_MICROPHONE          19
#define BTN_HEADPHONE           21

String serialInput;
String currentStatus;
bool microphoneMuted;
bool headphoneMuted;
bool buttonPressed;
int lastSerialAvailable;
int maxBrightness;
int waitingBrightness;

void ledStartupAnimation() {
  const int pwmChannels[] = { PWM_GREEN, PWM_YELLOW, PWM_RED, PWM_WHITE, PWM_MICROPHONE, PWM_HEADPHONE };
  const int size = sizeof(pwmChannels) / sizeof(int);
  const int animDelay = 125;

  for (int i = 0; i < size; i++) {
    if (i > 0) {
      ledcWrite(pwmChannels[i - 1], 0);
    }
    ledcWrite(pwmChannels[i], maxBrightness);
    delay(animDelay);
  }

  for (int i = size - 2; i >= 0; i--) {
    ledcWrite(pwmChannels[i + 1], 0);
    ledcWrite(pwmChannels[i], maxBrightness);
    delay(animDelay);
  }

  ledcWrite(pwmChannels[0], 0);
  delay(animDelay);
}

void emitStatus(String status) {
  Serial.println("status " + status);
}

void emitMicrophoneStatus(bool mute) {
  if (mute) {
    Serial.println("microphoneMuted true");
  } else {
    Serial.println("microphoneMuted false");
  }
}

void emitHeadphoneStatus(bool mute) {
  if (mute) {
    Serial.println("headphoneMuted true");
  } else {
    Serial.println("headphoneMuted false");
  }
}

void setStatusLed(int pwmChannel) {
  ledcWrite(PWM_GREEN, 0);
  ledcWrite(PWM_YELLOW, 0);
  ledcWrite(PWM_RED, 0);
  ledcWrite(PWM_WHITE, 0);

  if (pwmChannel >= 0) {
    ledcWrite(pwmChannel, maxBrightness);
  }
}

String split(String s, char parser, int index) {
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    } else parserCnt++;
  }
  return rs;
}

void setup() {
  currentStatus = "unknown";
  microphoneMuted = false;
  headphoneMuted = false;
  buttonPressed = false;
  maxBrightness = 255;
  waitingBrightness = 40;

  Serial.begin(BAUDRATE);

  pinMode(LED_GREEN,      OUTPUT);
  pinMode(LED_YELLOW,     OUTPUT);
  pinMode(LED_RED,        OUTPUT);
  pinMode(LED_WHITE,      OUTPUT);
  pinMode(LED_MICROPHONE, OUTPUT);
  pinMode(LED_HEADPHONE,  OUTPUT);

  ledcAttachPin(LED_GREEN, PWM_GREEN);
  ledcAttachPin(LED_YELLOW, PWM_YELLOW);
  ledcAttachPin(LED_RED, PWM_RED);
  ledcAttachPin(LED_WHITE, PWM_WHITE);
  ledcAttachPin(LED_MICROPHONE, PWM_MICROPHONE);
  ledcAttachPin(LED_HEADPHONE, PWM_HEADPHONE);

  ledcSetup(PWM_GREEN, 1000, 8);
  ledcSetup(PWM_YELLOW, 1000, 8);
  ledcSetup(PWM_RED, 1000, 8);
  ledcSetup(PWM_WHITE, 1000, 8);
  ledcSetup(PWM_MICROPHONE, 1000, 8);
  ledcSetup(PWM_HEADPHONE, 1000, 8);

  pinMode(BTN_GREEN,      INPUT_PULLUP);
  pinMode(BTN_YELLOW,     INPUT_PULLUP);
  pinMode(BTN_RED,        INPUT_PULLUP);
  pinMode(BTN_WHITE,      INPUT_PULLUP);
  pinMode(BTN_MICROPHONE, INPUT_PULLUP);
  pinMode(BTN_HEADPHONE,  INPUT_PULLUP);

  delay(1000);
  ledStartupAnimation();
  ledStartupAnimation();
}

void loop() {
  if (Serial.available() > 0) {
    lastSerialAvailable = millis();
    serialInput = Serial.readStringUntil('\n') + " ";

    if (serialInput.startsWith("name")) {
      Serial.write(_NAME);
    } else if (serialInput.startsWith("version")) {
      Serial.write(_VERSION);
    } else if (serialInput.startsWith("getbrightness")) {
      Serial.write(maxBrightness);
    } else if (serialInput.startsWith("setbrightness")) {
      int b = split(serialInput,' ',1).toInt();
      if (b >= 0 && b <= 255) {
        maxBrightness = b;
        waitingBrightness = int(b * 0.15);
      }
    } else if (serialInput.startsWith("getstatus")) {
      Serial.println(currentStatus);
    } else if (serialInput.startsWith("setstatus")) {
      if (split(serialInput,' ',1).equals("online")) {
        if (currentStatus != "online") {
          currentStatus = "online";
          setStatusLed(PWM_GREEN);
        }
      } else if (split(serialInput,' ',1).equals("afk")) {
        if (currentStatus != "afk") {
          currentStatus = "afk";
          setStatusLed(PWM_YELLOW);
        }
      } else if (split(serialInput,' ',1).equals("dnd")) {
        if (currentStatus != "dnd") {
          currentStatus = "dnd";
          setStatusLed(PWM_RED);
        }
      } else if (split(serialInput,' ',1).equals("invisible")) {
        if (currentStatus != "invisible") {
          currentStatus = "invisible";
          setStatusLed(PWM_WHITE);
        }
      } else if (split(serialInput,' ',1).equals("unknown")) {
        if (currentStatus != "unknown") {
          currentStatus = "unknown";
          setStatusLed(-1);
        }
      }

      if (split(serialInput,' ',2).equals("true")) {
        microphoneMuted = true;
        ledcWrite(PWM_MICROPHONE, maxBrightness);
      } else if (split(serialInput,' ',2).equals("false")) {
        microphoneMuted = false;
        ledcWrite(PWM_MICROPHONE, 0);
      }

      if (split(serialInput,' ',3).equals("true")) {
        headphoneMuted = true;
        ledcWrite(PWM_HEADPHONE, maxBrightness);
      } else if (split(serialInput,' ',3).equals("false")) {
        headphoneMuted = false;
        ledcWrite(PWM_HEADPHONE, 0);
      }
    }
  }

  if (digitalRead(BTN_GREEN) == LOW && !buttonPressed) {
    buttonPressed = true;
    ledcWrite(PWM_GREEN, waitingBrightness);
    emitStatus("online");
  } else if (digitalRead(BTN_YELLOW) == LOW && !buttonPressed) {
    buttonPressed = true;
    ledcWrite(PWM_YELLOW, waitingBrightness);
    emitStatus("afk");
  } else if (digitalRead(BTN_RED) == LOW && !buttonPressed) {
    buttonPressed = true;
    ledcWrite(PWM_RED, waitingBrightness);
    emitStatus("dnd");
  } else if (digitalRead(BTN_WHITE) == LOW && !buttonPressed) {
    buttonPressed = true;
    ledcWrite(PWM_WHITE, waitingBrightness);
    emitStatus("invisible");
  } else if (digitalRead(BTN_MICROPHONE) == LOW && !buttonPressed) {
    buttonPressed = true;
    if (microphoneMuted) {
      ledcWrite(PWM_MICROPHONE, waitingBrightness);
      emitMicrophoneStatus(false);
    } else {
      ledcWrite(PWM_MICROPHONE, waitingBrightness);
      emitMicrophoneStatus(true);
    }
  } else if (digitalRead(BTN_HEADPHONE) == LOW && !buttonPressed) {
    buttonPressed = true;
    if (headphoneMuted) {
      ledcWrite(PWM_HEADPHONE, waitingBrightness);
      emitHeadphoneStatus(false);
    } else {
      ledcWrite(PWM_HEADPHONE, waitingBrightness);
      emitHeadphoneStatus(true);
    }
  }

  if (digitalRead(BTN_GREEN) && digitalRead(BTN_YELLOW) && digitalRead(BTN_RED) && digitalRead(BTN_WHITE) && digitalRead(BTN_MICROPHONE) && digitalRead(BTN_HEADPHONE)) {
    buttonPressed = false;
  }

  if (millis() - lastSerialAvailable > CONNECTION_LOST_TIMEOUT) {
    currentStatus = "unknown";
    setStatusLed(-1); // all leds off
    ledcWrite(PWM_MICROPHONE, 0);
    ledcWrite(PWM_HEADPHONE, 0);
  }
}

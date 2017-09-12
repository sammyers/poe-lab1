const int LED_PINS[] = { 11, 12, 13 };
const int BUTTON_PIN = 10;
const int BLINK_INTERVAL = 100;

int ledStates[] = { LOW, LOW, LOW };
int lightMode = 0;
int buttonState = 0;

unsigned long previousMillis = 0;
int blinkState = 0;
int bounceState = 0;

// Morse Code Constants
int dotInterval = 100;
int dashInterval = 3 * dotInterval;
int elementPauseInterval = dotInterval;
int charPauseInterval = dashInterval;
int wordPauseInterval = 7 * dotInterval;

int signalIndex = 0;
boolean lightOn = false;

int sosSignals[] = { 1, 1, 1, 0, 2, 2, 2, 0, 1, 1, 1, 3 };
int sosSignalLength = sizeof(sosSignals) / sizeof(int);

void incrementValue(int *value, int modulus) {
  *value = (*value + 1) % modulus;
}

void incrementLightMode() {
  incrementValue(&lightMode, 5);
}

void writeToAllLeds(int val) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PINS[i], val);
  }
}

void executeIfTimeElapsed(void (*callback)(void *), int interval, void *data) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    callback(data);
  }
}

void allOn() {
  writeToAllLeds(HIGH);
}

void allOff() {
  writeToAllLeds(LOW);
}

void blinkCallback() {
  if (blinkState == LOW) {
    blinkState = HIGH;
  } else {
    blinkState = LOW;
  }
  writeToAllLeds(blinkState);
}

void blink() {
  executeIfTimeElapsed(blinkCallback, BLINK_INTERVAL, NULL);
}

void incrementBounceState() {
  incrementValue(&bounceState, 3);
}

void bounceCallback() {
  for (int i = 0; i < 3; i++) {
    if (i == bounceState) {
      digitalWrite(LED_PINS[i], HIGH);
    } else {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
  incrementBounceState();
}

void bounce() {
  executeIfTimeElapsed(bounceCallback, BLINK_INTERVAL, NULL);
}

void incrementMorseCodeElement(void *arrayLength) {
  incrementValue(&signalIndex, *((int *) arrayLength));
}

void turnMorseCodeLightOn() {
  allOn();
  lightOn = true;
}

void turnMorseCodeLightOff(void *signalLength) {
  allOff();
  lightOn = false;
  incrementMorseCodeElement(signalLength);
}

void morseCode(int *signals, int signalLength) {
  int signal = signals[signalIndex];
  int prevSignal = signalLength - 1;
  if (signalIndex > 0) {
    prevSignal = signals[signalIndex - 1];
  }

  switch (signal) {
    case 0: // characterpause
      executeIfTimeElapsed(incrementMorseCodeElement, charPauseInterval, (void *)(&signalLength));
      break;
    case 1: // dot
      if (lightOn) {
        executeIfTimeElapsed(turnMorseCodeLightOff, dotInterval, (void *)(&signalLength));
      } else {
        if (prevSignal == 1 || prevSignal == 2) {
          executeIfTimeElapsed(turnMorseCodeLightOn, elementPauseInterval, NULL);
        } else {
          executeIfTimeElapsed(turnMorseCodeLightOn, 0, NULL);
        }
      }
      break;
    case 2: // dash
      if (lightOn) {
        executeIfTimeElapsed(turnMorseCodeLightOff, dashInterval, (void *)(&signalLength));
      } else {
        if (prevSignal == 1 || prevSignal == 2) {
          executeIfTimeElapsed(turnMorseCodeLightOn, elementPauseInterval, NULL);
        } else {
          executeIfTimeElapsed(turnMorseCodeLightOn, 0, NULL);
        }
      }
      break;
    case 3: // wordpause
      executeIfTimeElapsed(incrementMorseCodeElement, wordPauseInterval, (void *)(&signalLength));
      break;
    default:
      break;
  }
}

void sos() {
  morseCode(sosSignals, sosSignalLength);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int currentButtonState = digitalRead(BUTTON_PIN);
  if (buttonState != currentButtonState && currentButtonState == HIGH) {
    incrementLightMode();
  }
  buttonState = currentButtonState;

  switch (lightMode) {
    case 0:
      allOff();
      break;
    case 1:
      allOn();
      break;
    case 2:
      blink();
      break;
    case 3:
      bounce();
      break;
    case 4:
      sos();
      break;
    default:
      break;
  }
}



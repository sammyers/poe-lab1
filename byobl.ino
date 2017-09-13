// Define pins
const int LED_PINS[] = { 11, 12, 13 };
const int BUTTON_PIN = 10;
const int SENSOR_PIN = A0;

// Initialize state variables
int ledStates[] = { LOW, LOW, LOW };
int lightMode = 0;
int buttonState = 0;

// Blinking and potentiometering state variables
unsigned long previousMillis = 0;
int blinkState = 0;
int bounceState = 0;
int blinkInterval = 100; // ms
int sensorValue = 0;

// Morse code constants
int dotInterval = 100;
int dashInterval = 3 * dotInterval;
int elementPauseInterval = dotInterval;
int charPauseInterval = dashInterval;
int wordPauseInterval = 7 * dotInterval;

int signalIndex = 0;
boolean lightOn = false;

int sosSignals[] = { 1, 1, 1, 0, 2, 2, 2, 0, 1, 1, 1, 3 };
int sosSignalLength = sizeof(sosSignals) / sizeof(int);

// Helper functions
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

/*
 * Calls provided function with the provided `data` argument if a given
 * interval (in milliseconds) has elapsed.
 *
 * Used for blink, bounce, and morse code modes.
 */
void executeIfTimeElapsed(void (*callback)(void *), int interval, void *data) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    callback(data);
  }
}

// Wrapper functions to turn on and off all LEDs
void allOn() {
  writeToAllLeds(HIGH);
}

void allOff() {
  writeToAllLeds(LOW);
}

// Toggles all lights on/off
void blinkCallback() {
  if (blinkState == LOW) {
    blinkState = HIGH;
  } else {
    blinkState = LOW;
  }
  writeToAllLeds(blinkState);
}

// Runs in main loop; toggles lights when blink intervals have elapsed
void blink() {
  executeIfTimeElapsed(blinkCallback, blinkInterval, NULL);
}

// bounceState keeps track of which LED is lit
void incrementBounceState() {
  incrementValue(&bounceState, 3);
}

// Toggles current LED off and next LED on
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

// Runs in main loop; handles light toggling when intervals have elapsed
void bounce() {
  executeIfTimeElapsed(bounceCallback, blinkInterval, NULL);
}

// Increments through array of morse code elements
void incrementMorseCodeElement(void *arrayLength) {
  incrementValue(&signalIndex, *((int *) arrayLength));
}

/*
 * Wrapper functions for allOn and allOff,
 * update morse code state variables accordingly
 */
void turnMorseCodeLightOn() {
  allOn();
  lightOn = true;
}

void turnMorseCodeLightOff(void *signalLength) {
  allOff();
  lightOn = false;
  incrementMorseCodeElement(signalLength);
}

/*
 * Takes in array of signals and length of array. Toggles all lights on/off
 * based on provided signals and current time.
 */
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

// Wrapper function to blink SOS in morse code
void sos() {
  morseCode(sosSignals, sosSignalLength);
}

// Sets things up (initializes input and output pins)
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  pinMode(BUTTON_PIN, INPUT);
}

/*
 * Runs until program terminates.
 *
 * Keeps track of current potentiometer value and updates blinkInterval.
 *
 * Keeps track of current button state and switches between modes.
 */
void loop() {
  sensorValue = analogRead(SENSOR_PIN);
  blinkInterval = 3 * sensorValue + 100;
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

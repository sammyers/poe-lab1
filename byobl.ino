const int ledPins[] = { 11, 12, 13 };
const int buttonPin = 10;
const int blinkInterval = 500;

int ledStates[] = { LOW, LOW, LOW };
int lightMode = 0;
unsigned long previousMillis = 0;
int buttonState = 0;

int blinkState = 0;

void incrementLightMode() {
  lightMode = (lightMode + 1) % 3;
}

void writeToAllLeds(int val) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPins[i], val);
  }
}

void blink() {
  unsigned long currentMillis = millis();
  Serial.print(currentMillis - previousMillis);
  Serial.println();

  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;

    if (blinkState == LOW) {
      blinkState = HIGH;
    } else {
      blinkState = LOW;
    }

    writeToAllLeds(blinkState);
  }
}

void allOff() {
  writeToAllLeds(LOW);
}

void allOn() {
  writeToAllLeds(HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int currentButtonState = digitalRead(buttonPin);
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
    default:
      break;
  }
  Serial.print(lightMode);
  Serial.println();
}



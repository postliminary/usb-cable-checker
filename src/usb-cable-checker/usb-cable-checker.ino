/**
   Scenarios this needs to test
   1. No connection (A =/=> A)
   2. Incorrect connection (A ===> B)
   3. Bridged connections (A ===> [A,B])
*/
enum PINS {
  VCC_SOURCE = 11,
  DATA_POS_SOURCE = 10,
  DATA_NEG_SOURCE = 9,
  GND_SOURCE = 8,
  VCC_TARGET = 7,
  DATA_POS_TARGET = 6,
  DATA_NEG_TARGET = 5,
  GND_TARGET = 4,
  BUTTON = 2
};

enum CHECKER_STATE {
  RESET,
  WAITING,
  CHECKING,
  FAILED
};

const int USB_SOURCES[] = {
  VCC_SOURCE,
  DATA_POS_SOURCE,
  DATA_NEG_SOURCE,
  GND_SOURCE
};
const int USB_SOURCES_COUNT = sizeof(USB_SOURCES) / sizeof(USB_SOURCES[0]);

const int USB_TARGETS[] = {
  VCC_TARGET,
  DATA_POS_TARGET,
  DATA_NEG_TARGET,
  GND_TARGET
};
const int USB_TARGETS_COUNT = sizeof(USB_TARGETS) / sizeof(USB_TARGETS[0]);;

int checkerState = WAITING;
int failure = 0;

void setup() {
  pinMode(VCC_SOURCE, OUTPUT);
  pinMode(DATA_POS_SOURCE, OUTPUT);
  pinMode(DATA_NEG_SOURCE, OUTPUT);
  pinMode(GND_SOURCE, OUTPUT);
  pinMode(VCC_TARGET, INPUT);
  pinMode(DATA_POS_TARGET, INPUT);
  pinMode(DATA_NEG_TARGET, INPUT);
  pinMode(GND_TARGET, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonHandler, RISING);
}

void loop() {
  switch (checkerState) {
    case WAITING:
      break;
    case CHECKING:
      checkConnections();
      break;
    case FAILED:
      uhOh();
      break;
    default:
      reset();
      break;
  }
}

void buttonHandler() {
  if (checkerState == WAITING) {
    checkerState = CHECKING;
  } else {
    checkerState = RESET;
  }
}

void reset() {
  resetSources();
  failure = 0;
  checkerState = WAITING;
}

void checkConnections() {
  // Iterate through all the SOURCES
  for (int i = 0; i < USB_SOURCES_COUNT; i++) {
    resetSources();
    
    // Check for a HIGH signal for the connection IO pins
    digitalWrite(USB_SOURCES[i], HIGH);
    delay(500);

    for (int j = 0; j < USB_TARGETS_COUNT; j++) {
      // Should only read HIGH for matching IN/OUT
      int assertedState = i == j ? HIGH : LOW;
      int connectionState = digitalRead(USB_TARGETS[j]);
      if (connectionState != assertedState) {
        failure = USB_SOURCES[i];
        checkerState = FAILED;
        return;
      }
    }
  }
}

void resetSources() {
  for (int i = 0; i < USB_SOURCES_COUNT; i++) {
    digitalWrite(USB_SOURCES[i], LOW);
  }
}

void uhOh() {
  digitalWrite(failure, HIGH);
  delay(250);
  digitalWrite(failure, LOW);
  delay(250);
}

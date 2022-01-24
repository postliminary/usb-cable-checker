/**
   Scenarios this circuit needs to test
   1. No connection (A =/=> A)
   2. Incorrect connection (A ===> B)
   3. Bridged connections (A ===> [A,B])
*/
enum PINS {
  VCC_OUT = 11,
  DATA_POS_OUT = 10,
  DATA_NEG_OUT = 9,
  GND_OUT = 8,
  VCC_IN = 7,
  DATA_POS_IN = 6,
  DATA_NEG_IN = 5,
  GND_IN = 4,
  BUTTON = 2
};

enum CHECKER_STATE {
  RESET,
  WAITING,
  CHECKING,
  FAILED
};

const int USB_OUTPUTS[] = {
  VCC_OUT,
  DATA_POS_OUT,
  DATA_NEG_OUT,
  GND_OUT
};
const int USB_OUTPUTS_COUNT = sizeof(USB_OUTPUTS) / sizeof(USB_OUTPUTS[0]);

const int USB_INPUTS[] = {
  VCC_IN,
  DATA_POS_IN,
  DATA_NEG_IN,
  GND_IN
};
const int USB_INPUTS_COUNT = sizeof(USB_INPUTS) / sizeof(USB_INPUTS[0]);;

int checkerState = WAITING;
int failure = 0;

void setup() {
  pinMode(VCC_OUT, OUTPUT);
  pinMode(DATA_POS_OUT, OUTPUT);
  pinMode(DATA_NEG_OUT, OUTPUT);
  pinMode(GND_OUT, OUTPUT);
  pinMode(VCC_IN, INPUT);
  pinMode(DATA_POS_IN, INPUT);
  pinMode(DATA_NEG_IN, INPUT);
  pinMode(GND_IN, INPUT);
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
  resetOutputs();
  failure = 0;
  checkerState = WAITING;
}

void checkConnections() {
  // Iterate through all the outputs
  for (int i = 0; i < USB_OUTPUTS_COUNT; i++) {
    resetOutputs();
    
    // Check for a HIGH signal for the connection IO pins
    digitalWrite(USB_OUTPUTS[i], HIGH);
    delay(500);

    for (int j = 0; j < USB_INPUTS_COUNT; j++) {
      // Should only read HIGH for matching IN/OUT
      int assertedState = i == j ? HIGH : LOW;
      int connectionState = digitalRead(USB_INPUTS[j]);
      if (connectionState != assertedState) {
        failure = USB_OUTPUTS[i];
        checkerState = FAILED;
        return;
      }
    }
  }
}

void resetOutputs() {
  for (int i = 0; i < USB_OUTPUTS_COUNT; i++) {
    digitalWrite(USB_OUTPUTS[i], LOW);
  }
}

void uhOh() {
  digitalWrite(failure, HIGH);
  delay(250);
  digitalWrite(failure, LOW);
  delay(250);
}

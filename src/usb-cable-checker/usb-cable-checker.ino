/**
 * Scenarios this circuit needs to test
 * 1. No connection (A =X=> A)
 * 2. Incorrect connection (A ===> B)
 * 3. Bridged connections (A ===> [A,B])
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
  WAITING,
  START,
  CHECK_VCC,
  CHECK_DATA_POS,
  CHECK_DATA_NEG,
  CHECK_GND,
  SUCCESS,
  ERROR_VCC,
  ERROR_DATA_POS,
  ERROR_DATA_NEG,
  ERROR_GND
};

int checkerState = WAITING;

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
  switch(checkerState) {
    case WAITING:
      waiting();
      break;
    case START:
      start();
      break;
    case CHECK_VCC:
      checkConnection(VCC_OUT, VCC_IN, CHECK_DATA_POS, ERROR_VCC);
      break;
    case CHECK_DATA_POS:
      checkConnection(DATA_POS_OUT, DATA_POS_IN, CHECK_DATA_NEG, ERROR_DATA_POS);
      break;
    case CHECK_DATA_NEG:
      checkConnection(DATA_NEG_OUT, DATA_NEG_IN, CHECK_GND, ERROR_DATA_NEG);
      break;
    case CHECK_GND:
      checkConnection(GND_OUT, GND_IN, SUCCESS, ERROR_GND);
      break;
    case SUCCESS:
      success();
      break;
    case ERROR_VCC:
      uhOh(VCC_OUT);
      break;
    case ERROR_DATA_POS:
      uhOh(DATA_POS_OUT);
      break;
    case ERROR_DATA_NEG:
      uhOh(DATA_NEG_OUT);
      break;
    case ERROR_GND:
      uhOh(GND_OUT);
      break;
    default:
      checkerState = WAITING;
      break;
  }
}

void waiting() {
  resetAll();
}

void start() {
  checkerState = CHECK_VCC;
}

void checkConnection(int pinOut, int pinIn, int nextState, int errorState) {
  resetAll();
  
  // Checks for a digital signal between the pins
  digitalWrite(pinOut, HIGH);
  delay(500);
  
  int connectionState = digitalRead(pinIn);
  if (connectionState == HIGH) {
    checkerState = nextState;
  } else {
    checkerState = errorState;
  }
}

void success() {
  // Success "animation"
  digitalWrite(VCC_OUT, HIGH);
  digitalWrite(DATA_POS_OUT, HIGH);
  digitalWrite(DATA_NEG_OUT, HIGH);
  digitalWrite(GND_OUT, HIGH);
}

const long uhOhInterval = 500;
unsigned int uhOhTimer = 0;
int uhOhState = LOW;

void uhOh(int pinOut) {
  unsigned int currentTimer = millis();
  if (currentTimer - uhOhTimer >= uhOhInterval) {
    uhOhTimer = currentTimer;

    if (uhOhState == LOW) {
      uhOhState = HIGH;
    } else {
      uhOhState = LOW;
    }

    digitalWrite(pinOut, uhOhState);
  }
}

void buttonHandler() {
  switch(checkerState) {
    case SUCCESS:
      checkerState = WAITING;
      break;
    case WAITING:
      checkerState = START;
      break;
  }
}

void resetAll() {
  digitalWrite(VCC_OUT, LOW);
  digitalWrite(DATA_POS_OUT, LOW);
  digitalWrite(DATA_NEG_OUT, LOW);
  digitalWrite(GND_OUT, LOW);
}

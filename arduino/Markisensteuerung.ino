// Input Pins for Switch Markise Up/Down
const int SwMarkUp = 9;
const int SwMarkDown = 8;
// Input Pins for Switch Jalosie Up/Down
const int SwJalUp = 7;
const int SwJalDown = 6;

// Output Pins
const int MarkOn = 10;
const int MarkDown = 11;
const int JalOn = 12;
const int JalDown = 13;
int MarkUpState = 0;
int MarkDownState = 0;
int JalUpState = 0;
int JalDownState = 0;

//States
const int MOTOR_POSITION_UNKNOWN = 0;
const int MOTOR_POSITION_UP = 1;
const int MOTOR_POSITION_DOWN = 2;
const int MOTOR_MOVING_UP = 3;
const int MOTOR_MOVING_DOWN = 4;


//mark
int MarkState = 0;
long mark_time_for_one_task = 6000;
long mark_time_until_finish = 0;
long mark_time_while_mute = 0;

//jal
int JalState = 0;


void setup() {
  // Initialize In-/Outputs
  pinMode(SwMarkUp, INPUT_PULLUP);
  pinMode(SwMarkDown, INPUT_PULLUP);
  pinMode(SwJalUp, INPUT_PULLUP);
  pinMode(SwJalDown, INPUT_PULLUP);
  pinMode(MarkOn, OUTPUT);
  pinMode(MarkDown, OUTPUT);
  pinMode(JalOn, OUTPUT);
  pinMode(JalDown, OUTPUT);
  digitalWrite(MarkOn, LOW);
  digitalWrite(MarkDown, LOW);
  digitalWrite(JalOn, LOW);
  digitalWrite(JalDown, LOW);
  Serial.begin(9600);

}

void loop() {
  // Read Input
  MarkUpState = digitalRead(SwMarkUp);
  MarkDownState = digitalRead(SwMarkDown);
  JalUpState = digitalRead(SwJalUp);
  JalDownState = digitalRead(SwJalDown);

  if (mark_time_while_mute > millis()) {
  return;
}

//Markise
if (MarkUpState == LOW)  { //Pressing "Move Up"
  if (MarkState == MOTOR_MOVING_DOWN) {
      stopMarkMovement(MOTOR_POSITION_UNKNOWN);
      mark_time_while_mute = millis() + 400;
      return;
    }
    if (canMarkMove(MOTOR_POSITION_UP)) {
      if (MarkState != MOTOR_MOVING_UP) {
        startMarkMovement(MOTOR_MOVING_UP);
      }
    }

  } else if (MarkDownState == LOW) { //Pressing "Move Down"
  if (MarkState == MOTOR_MOVING_UP) {
      stopMarkMovement(MOTOR_POSITION_UNKNOWN);
      mark_time_while_mute = millis() + 400;
      return;
    }
    if (canMarkMove(MOTOR_POSITION_DOWN)) {
      if (MarkState != MOTOR_MOVING_DOWN) {
        startMarkMovement(MOTOR_MOVING_DOWN);
      }
    }
  }


  if (mark_time_until_finish <= millis() && mark_time_until_finish > 0) {
    switch (MarkState) {
      case MOTOR_MOVING_UP:
        stopMarkMovement(MOTOR_POSITION_UP);
        break;
      case MOTOR_MOVING_DOWN:
        stopMarkMovement(MOTOR_POSITION_DOWN);
        break;
    }
  }

  /*
    // Markise
    // Activated Input is "LOW"
    if (MarkUpState == LOW)  {
      digitalWrite(MarkOn, HIGH);
    } else {
      if (MarkDownState == HIGH) {
        digitalWrite(MarkOn, LOW);
      }
      if (MarkDownState == LOW) {
        digitalWrite(MarkDown, HIGH);
        delay(150); // Motor Protection
        digitalWrite(MarkOn, HIGH);
      } else {
        digitalWrite(MarkOn, LOW);
        delay(150);
        digitalWrite(MarkDown, LOW);
      }
    }*/

  // Jalosie
  if (JalUpState == LOW) {
  digitalWrite(JalOn, HIGH);
  } else {
    if (JalDownState == HIGH) {
      digitalWrite(JalOn, LOW);
    }
    if (JalDownState == LOW) {
      digitalWrite(JalDown, HIGH);
      delay(150);
      digitalWrite(JalOn, HIGH);
    } else {
      digitalWrite(JalOn, LOW);
      delay(150);
      digitalWrite(JalDown, LOW);
    }
  }

  //Motor protection; uncharge capacitor over the coil
  delay(400);
}


//mark
boolean canMarkMove(int requested) {
  switch (requested) {
    case MOTOR_POSITION_UNKNOWN:
      return false;
    case MOTOR_POSITION_UP:
      return MarkState != MOTOR_POSITION_UP;
    case MOTOR_POSITION_DOWN:
      return MarkState != MOTOR_POSITION_DOWN;
  }
  return true;
}

void stopMarkMovement(int destination) {
  mark_time_until_finish = 0; //Set destination time to 0 -> it's not active anymore
  digitalWrite(MarkOn, LOW);
  delay(150);
  digitalWrite(MarkDown, LOW);
  MarkState = destination;
}

void startMarkMovement(int motor_state) {
  MarkState = motor_state; //Set current motor state. Either "MOTOR_MOVING_DOWN" or "MOTOR_MOVING_UP"
  int mark_down_type = (motor_state == MOTOR_MOVING_UP) ? LOW : HIGH; //Set the relais depending on the movement direction; UP -> LOW
  digitalWrite(MarkDown, mark_down_type); //Activate relais
  delay(150);
  digitalWrite(MarkOn, HIGH); //Activate motor
  mark_time_until_finish = millis() + mark_time_for_one_task; //Set destination time
}

//jal








void debug(String text) {
  Serial.println(text);
}



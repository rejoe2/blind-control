#include <Wgs.h>



// Input Pins for Switch Markise Up/Down
const int SwMarkUp = 8;
const int SwMarkDown = 9;
// Input Pins for Switch Jalosie Up/Down
const int SwJalUp = 7;
const int SwJalDown = 6;
//Notfall
const int SwEmergency = 5;


// Output Pins
const int MarkOn = 12;
const int MarkDown = 13;
const int JalOn = 10;
const int JalDown = 11;
int MarkUpState = 0;
int MarkDownState = 0;
int JalUpState = 0;
int JalDownState = 0;
int EmergencyState = 0;

Wgs mark(MarkOn, MarkDown, 55000);
Wgs jal(JalOn, JalDown, 55000);

void setup()
{
  // Initialize In-/Outputs
  pinMode(SwMarkUp, INPUT_PULLUP);
  pinMode(SwMarkDown, INPUT_PULLUP);
  pinMode(SwJalUp, INPUT_PULLUP);
  pinMode(SwJalDown, INPUT_PULLUP);
  pinMode(SwEmergency, INPUT_PULLUP);
  pinMode(MarkOn, OUTPUT);
  pinMode(MarkDown, OUTPUT);
  pinMode(JalOn, OUTPUT);
  pinMode(JalDown, OUTPUT);
  digitalWrite(MarkOn, HIGH);
  digitalWrite(MarkDown, HIGH);
  digitalWrite(JalOn, HIGH);
  digitalWrite(JalDown, HIGH);
  
  Serial.begin(9600);
}

void loop()
{

  MarkUpState = digitalRead(SwMarkUp);
  MarkDownState = digitalRead(SwMarkDown);
  JalUpState = digitalRead(SwJalUp);
  JalDownState = digitalRead(SwJalDown);
  EmergencyState = digitalRead(SwEmergency);
  
  
  if(EmergencyState == LOW) { //Rain -> disable mark
  //Notfall
  mark.setDisable(true);
  
  }else{
  mark.setDisable(false);
  }
  
  mark.loop(MarkUpState == LOW, MarkDownState == LOW);
  jal.loop(JalUpState == LOW, JalDownState == LOW);
  
}

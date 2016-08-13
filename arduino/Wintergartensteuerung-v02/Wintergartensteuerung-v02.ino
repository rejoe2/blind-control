#include <Wgs.h>

// For RTC
#include "Wire.h" //warum andere Schreibweise ?
#define DS3231_I2C_ADDRESS 0x68

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


byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}


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

  Wire.begin();
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






// Das muss raus !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  displayTime(); // display the real-time clock data on the Serial Monitor,
  delay(1000); // every second

  
}








//////////////////////Test 
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  }
}


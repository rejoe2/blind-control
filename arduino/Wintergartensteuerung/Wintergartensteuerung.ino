//V006
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
const int JalOn = 10;
const int JalDown = 11;
const int JalRevers = 12;
const int MarkOn = 13;
const int MarkDown = 14;

int MarkUpState = 0;
int MarkDownState = 0;
int JalUpState = 0;
int JalDownState = 0;
int JalReverseState = 0;
int EmergencyState = 0;

//autostart
const int autostart_time = 9;

const int autostart_check_delay = 200; //in ticks
int autostart_check_tick = 200; //in ticks
boolean autostart_done = false;






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
  Serial.begin(57600);
}

void loop()
{

    
    //böse
    delay (300);
    displayTime();



  bool button_mark_up = digitalRead(SwMarkUp) == LOW;
  bool button_mark_down = digitalRead(SwMarkDown) == LOW;
  bool button_jal_up = digitalRead(SwJalUp) == LOW;
  bool button_jal_down = digitalRead(SwJalDown) == LOW;  
  bool emergency = digitalRead(SwEmergency) == LOW; //Current use: in case of rain

  mark.setDisable(emergency);  

  //Autostart code
  autostart_check_tick++;
  if(autostart_check_tick >= autostart_check_delay){
    autostart_check_tick = 0;  
    
    
    // Darf das mehrfach sein ?
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    


    if(autostart_done){ //Already done
      if(hour > autostart_time){
        autostart_done = false;
      }
    }else{
      if(hour == autostart_time &! emergency){
        button_mark_down = true;
        autostart_done = true;
      }
    }

  }
  

  mark.loop(button_mark_up, button_mark_down);
  jal.loop(button_jal_up, button_jal_down);


}








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
  Serial.println(year, DEC);
  

}


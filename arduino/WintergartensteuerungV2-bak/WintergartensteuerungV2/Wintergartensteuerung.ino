// (C) www.neuby.de
//V2.001
// Enable debug prints to serial monitor
#define MY_DEBUG
// Enable RS485 transport layer
#define MY_RS485
// Define this to enables DE-pin management on defined pin
#define MY_RS485_DE_PIN 2
// Set RS485 baud rate to use
#define MY_RS485_BAUD_RATE 9600
#define MY_NODE_ID 118
//#define MY_TRANSPORT_RELAXED
#define MY_TRANSPORT_WAIT_READY_MS 2000

#include <MySensors.h>
#include <Arduino.h>
#include <SPI.h>
#include <Bounce2.h>
#include <Wgs.h>
// For RTC
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

#define CHILD_ID_LIGHT 0
#define CHILD_ID_Rain 1   // Id of the sensor child
unsigned long SLEEP_TIME = 10; // Sleep time between reads (in milliseconds)

// Initialize motion message
MyMessage msgRain(CHILD_ID_Rain, V_RAIN);
#define DIGITAL_INPUT_SENSOR 3   // The digital input you attached your rain sensor.  (Only 2 and 3 generates interrupt!)




// Buttons
Bounce debounceJalUp    = Bounce();
Bounce debounceJalDown  = Bounce();
Bounce debounceMarkEmergency  = Bounce();
Bounce debounceMarkUp    = Bounce();
Bounce debounceMarkDown  = Bounce();




// Input Pins for Switch Markise Up/Down
const int SwMarkUp = 8;
const int SwMarkDown = 9;
// Input Pins for Switch Jalosie Up/Down
const int SwJalUp = 7;
const int SwJalDown = 6;
//Notfall
const int SwEmergency = 5;


// Output Pins
const int JalUp = 10;
const int JalDown = 11;
//const int JalRevers = 12;
const int MarkUp = 13;
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


Wgs mark(MarkUp, MarkDown, 55000);
Wgs jal(JalUp, JalDown, 65000);

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
  pinMode(MarkUp, OUTPUT);
  pinMode(MarkDown, OUTPUT);
  pinMode(JalUp, OUTPUT);
  pinMode(JalDown, OUTPUT);
  digitalWrite(MarkUp, HIGH);
  digitalWrite(MarkDown, HIGH);
  digitalWrite(JalUp, HIGH);
  digitalWrite(JalDown, HIGH);

  // After setting up the button, setup debouncer
  debounceJalUp.attach(SwJalUp);
  debounceJalUp.interval(5);
  debounceJalDown.attach(SwJalDown);
  debounceJalDown.interval(5);
  debounceMarkUp.attach(SwMarkUp);
  debounceMarkUp.interval(5);
  debounceMarkDown.attach(SwMarkDown);
  debounceMarkDown.interval(5);

  debounceMarkEmergency.attach(SwEmergency);
  debounceMarkEmergency.interval(5);




  Wire.begin();
  //  Serial.begin(57600);

}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Light Rain Sensor", "0.1.0");

  // Register all sensors to gateway (they will be created as child devices)
  present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);
  present(CHILD_ID_Rain, S_RAIN);

}





void loop()
{
  // Read digital motion value
  bool tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;
  Serial.println(tripped);
  send(msgRain.set(tripped?"0":"1"));  // Send tripped value to gw
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

 sleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), CHANGE, SLEEP_TIME);

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

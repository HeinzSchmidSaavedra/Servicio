#include <SD.h>
#include <SPI.h>
#include <Sodaq_DS3231.h>
#include <LowPower.h>

//switched power lines pin
#define SWITCHED_POWER 22

//Error flag and error LED pin
#define ERROR_LED 9
bool error;

//Operation Success pin
#define SUCCESS_LED 8
bool success;
//SD card pin 
#define sd= 10;
//DateTime variable for the time
DateTime now;

//create a string buffer for SD card and printing info to the debugging serial port
String string_buffer;

//File object variable
File file;
//COnfiguration of battery voltage variables
int batteryPin = A6;    // on the Mayfly board, pin A6 is connected to a resistor divider on the battery input; R1 = 10 Mohm, R2 = 2.7 Mohm
int batterysenseValue = 0;  // variable to store the value coming from the analogRead function
float batteryvoltage;       // the battery voltage as calculated by the formula below

//pin for the RTC alarm interrupt
int interruptPin = A7;
//pin for the pressure sensor
#define transductor_voltage = A2;
// function for converting the voltage values into pressure values
float pressure(volt){
  ///////////
  //work in progress
  ////////
}
// Interrupt service routine for RTC alarm
void INT0_ISR()
{
  //nothig here, ,just an interrupt to awake the device
}
void setup(){
  //Serial.begin(9600);
  //configure switched power, error and success pins
    pinMode(SWITCHED_POWER, OUTPUT);
    digitalWrite(SWITCHED_POWER, LOW);
    pinMode(ERROR_LED, OUTPUT);
    digitalWrite(ERROR_LED, LOW);
    pinMode(SUCCESS_LED, OUTPUT);
    digitalWrite(SUCCESS_LED, LOW);
    //If the SD card doesn't initialize 
    while (!SD.begin(sd)) {
      digitalWrite(ERROR_LED, HIGH);
      delay(500);//espera antes de volver a intentarlo 
    }
    //Configure the interrupt to awake the device
    pinMode(interruptPin, INPUT_PULLUP);
    PcInt::attachInterrupt(interruptPin, INT0_ISR);
    //initialize rtc
    rtc.begin();
    rtc.enableInterrupts(EveryHour);

}
void loop(){
  

}

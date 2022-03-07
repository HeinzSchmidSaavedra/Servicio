#include <SD.h>
#include <SPI.h>
#include <Sodaq_PcInt.h>
#include <Sodaq_DS3231.h>
#include <LowPower.h>
#define DEBUG

//switched power lines pin
#define SWITCHED_POWER 22

//Error flag and error LED pin
#define ERROR_LED 9
bool error;

//Operation Success pin
#define SUCCESS_LED 8
bool success;
//SD card pin 
#define sd 10
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
#define transductor_voltage A2
// function for converting the voltage values into pressure values
//float pressure(){
  ///////////
  //work in progress
  ////////
//}
// Interrupt service routine for RTC alarm
void INT0_ISR()
{
  //nothig here, ,just an interrupt to awake the device
}
void setup(){
  #if defined DEBUG
  Serial.begin(9600);
  #endif
  //configure switched power, error and success pins
    pinMode(SWITCHED_POWER, OUTPUT);
    digitalWrite(SWITCHED_POWER, LOW);
    pinMode(ERROR_LED, OUTPUT);
    digitalWrite(ERROR_LED, LOW);
    pinMode(SUCCESS_LED, OUTPUT);
    digitalWrite(SUCCESS_LED, LOW);
    //If the SD card doesn't initialize 
     while(!SD.begin(sd)) {
      #if defined DEBUG
        Serial.print("SD card reding failed");
      #endif
      digitalWrite(ERROR_LED, HIGH);
      delay(500);//espera antes de volver a intentarlo 
    }
    digitalWrite(ERROR_LED, LOW);
    #if defined DEBUG
      Serial.print("SD card reading succesful");
    #endif
    
    //Configure the interrupt to awake the device
    pinMode(interruptPin, INPUT_PULLUP);
    PcInt::attachInterrupt(interruptPin, INT0_ISR);
    //initialize rtc
    rtc.begin();
    rtc.enableInterrupts(EveryHour);

}
void loop(){
  //turn the switched source on
    digitalWrite(SWITCHED_POWER, HIGH);
    float volt = analogRead(transductor_voltage);
    //power sensor off as soon as it finishes reading
    digitalWrite(SWITCHED_POWER, LOW);
    string_buffer = "";
    batterysenseValue = analogRead(batteryPin); 
    ////////
    ///waiting for verification
    batteryvoltage = (3.3/1023.) * 4.7037 * batterysenseValue;
    ///
    //////
    now = rtc.now();
    //
    string_buffer = String(now.date()) + "_" + String(now.month()) + ".csv";

    // section to verify if the file exists, if it doesn't then create it and generate the text file header
    if (!SD.exists(string_buffer))
    {
      file = SD.open(string_buffer, FILE_WRITE);
      if (file)
      {
        file.println(F("Fecha/hora, presión, Bateria"));
        file.close();
      }
      else error = true;
    }
    file = SD.open(string_buffer, FILE_WRITE);
    string_buffer = ""; 
    now.addToString(string_buffer);
    string_buffer.concat("," + String(volt)+","+ String(batteryvoltage));
    #if defined DEBUG
    Serial.println(string_buffer);
    #endif
    if (file)
    {
      file.println(string_buffer);
      file.close();
      #if defined DEBUG
      Serial.println(F("Succesfully written to the SD"));
      #endif
      error = false;
    }
    else error = true;
    //update error pin status
    if (error){
      digitalWrite(ERROR_LED, HIGH);
    }
     //This section clears the alarm flag of the RTC and puts the device in deep sleep
    rtc.clearINTStatus();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);


}

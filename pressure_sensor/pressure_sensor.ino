#include <SD.h>
#include <Sodaq_PcInt.h>
#include <Sodaq_DS3231.h>
#include <LowPower.h>
#define DEBUG
// LED pin
#define LED 13


//SD card pin 
#define sd 10
//DateTime variable for the time
DateTime now;

//create a string buffer for SD card and printing info to the debugging serial port
String string_buffer;

//File object variable
File file;
//COnfiguration of battery voltage variables
#define batteryPin  A1  // on the Mayfly board, pin A6 is connected to a resistor divider on the battery input; R1 = 10 Mohm, R2 = 2.7 Mohm
float battery = 0;  // variable to store the % of battery
// Function that converts the value of voltage read by the device into the real voltage
float batteryvoltage(){ 
  float batteryValue = analogRead(batteryPin); 
  //batteryvoltage = (3.3/1023.) * 4.7037 * batterysenseValue;
  float conversion_battery = 2*map(batteryValue,0,1023,0,1.85);
  return conversion_battery;
}
//pin for the RTC alarm interrupt
int interruptPin = A5;
//pin for the pressure sensor
#define transductor_voltage A2
// function for converting the voltage values into pressure values
float conversion(){
  float volt = analogRead(transductor_voltage);
  //Unit conversion from voltage to PSI, assuming that 0V = 0PSI and 5v = 150PSI
  float value = map(volt, 0, 1023, 0, 150);
  return value;
}
float pressure;
// Interrupt service routine for RTC alarm
void INT0_ISR()
{
  //nothig here, ,just an interrupt to awake the device
}
// Interrupt service routine for RTC alarm
void setup(){
  #if defined DEBUG
  Serial.begin(9600);
  #endif
  //configure switched power, error and success pins
    pinMode(LED, OUTPUT);
    pinMode(sd, OUTPUT);
    digitalWrite(LED, LOW);
    //If the SD card doesn't initialize 
     while(!SD.begin(sd)) {
      #if defined DEBUG
        Serial.println("SD card reding failed");
      #endif
      digitalWrite(LED, HIGH);
      delay(500);//espera antes de volver a intentarlo 
    }
    digitalWrite(LED, LOW);
    #if defined DEBUG
      Serial.println("SD card reading succesful");
    #endif
    
    //Configure the interrupt to awake the device
    pinMode(interruptPin, INPUT_PULLUP);
    PcInt::attachInterrupt(interruptPin, INT0_ISR);
    //initialize rtc
    rtc.begin();
    #if defined DEBUG 
      rtc.enableInterrupts(EveryMinute);
    #else
      rtc.enableInterrupts(EveryHour);
    #endif
}
void loop(){
    //Reads save the info sended form the transducer, convert it to PSI and stores it 
    pressure= conversion();
    battery = batteryvoltage();
    
    //cleans  the sting buffer
    string_buffer = "";
    
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
      else{
        //In case something goes wrong warn me with the LED
        int cont = 0;
        while(cont<2){
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED,LOW);
        delay(100);
        cont+=1;
        }
      }
    }
    file = SD.open(string_buffer, FILE_WRITE);
    string_buffer = "";    
    now.addToString(string_buffer);
    string_buffer.concat("," + String(pressure)+","+ String(battery));
    #if defined DEBUG
      Serial.println(string_buffer);
    #endif
    if (file){
      file.println(string_buffer);
      file.close();
      #if defined DEBUG
      Serial.println(F("Succesfully written to the SD"));
      #endif
    }
    else{
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
     //This section clears the alarm flag of the RTC and puts the device in deep sleep
    rtc.clearINTStatus();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);


}

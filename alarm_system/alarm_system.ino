#include <IRremote.h>

// EACH CONTROLLER MIGHT HAVE A DIFFERENT BUTTON LAYOUT
// WE NEED TO INDIVIDUALLY CHECK WHAT BUTTON TO USE
// #define BTN_ON 0xFF6897
// #define BTN_OFF 0xFF9867

#define BTN_ON 0XE916FF00
#define BTN_OFF 0xF20DFF00
#define BUZZER_PIN 9

// declare and init variables
const int IR_RECEIVE_PIN = 10;
const int LED_BLUE = 8; // USE a 100 ohm resistor
const int LED_RED = 6; // USE a 100 ohm resistor
const int LED_GREEN = 7; // USE a 100 ohm resistor

const int MAGNET_SENSOR = 2;
int sensorState = 0;
int alarmTriggered = 0;  // this state holds data when the alarm is triggered in mangent sensor
bool alarmState = false; // for handling the code when the alarm is on
bool isAlarmTriggered = false; // this bool helps with turning the lights and buzzer on

const unsigned long eventTime_1_IRRemote = 400; // interval in ms
const unsigned long eventTime_2_MagnetRead = 900; // interval in ms
unsigned long prevTime_1 = 0; // this helps keep our program time to aid with running multiple sensors
unsigned long prevTime_2 = 0; // this helps keep our program time to aid with running multiple sensors

uint32_t prev; // will store a 32bit unsinged int value


/**
 * Code set up as usual
 * MAKE SURE TO PLACE SET UP CODE FOR ALL COMPONENTS HERE
 */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // set up port
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // start the reciever
  pinMode(LED_RED, OUTPUT); // start the RED_LED
  pinMode(LED_GREEN,OUTPUT); // start the LED_GREEN
  pinMode(LED_BLUE,OUTPUT); // start the LED_BLUE
  pinMode(MAGNET_SENSOR, INPUT_PULLUP); // start the magnetic sensor
  pinMode(BUZZER_PIN, OUTPUT); // start the buzzer 
  Serial.print("Enabling pin: "); // test to see if our reciever is in the correct spot
  Serial.println(IR_RECEIVE_PIN); // test to see if our reciever is in the correct spot

}

void loop() {
  unsigned long currTime = millis();

  // Since these control the state of the alarm they
  // should always be reading
  readFromIrRemote();
  alarmIsSet();
  
  // setting up the first event
  if(currTime - prevTime_1 >= eventTime_1_IRRemote){
    Serial.println("IRRemote ");
    //readFromIrRemote();
    readFromIrRemote();
    IrReceiver.resume(); // recieve next value
    // "Resetting" time for next event
    prevTime_1 = currTime;
  }

   // setting up the first event
  if(currTime - prevTime_2 >= eventTime_2_MagnetRead){
    Serial.println("Magnet Sensor");
    if(alarmState == 1){
        readFromMagnetSensor(MAGNET_SENSOR);
        IrReceiver.resume(); // recieve next value
    }
    else{
      readFromIrRemote();
      IrReceiver.resume(); // recieve next value
    }
      
    // "Resetting" time for next event
    prevTime_2 = currTime;
  }
  IrReceiver.resume(); // recieve next value
}


/**
 * This function will handle the IR Reciever and IR Remote Controller
 * interactions.
 * 
 * If the star button is pressed on the remote then the alarm will be armed.
 * To dissarm the alarm, we must press the pound button on the remote
 * 
 * Author: Robert Ortiz
 * 2/22/21
 */

 void  readFromIrRemote(){
  // if we receive some signal from the remote then we should handle that
  if(IrReceiver.decode()){

    // UNCOMMENT LINE BELOW TO PRINT REMOTE VALUES TO CONSOLE
    Serial.println(IrReceiver.decodedIRData.decodedRawData,HEX);
    
    // The remote by default will read zero so we should treat that as noise
    if(IrReceiver.decodedIRData.decodedRawData == 0xFFFFFFFF){
      IrReceiver.decodedIRData.decodedRawData = prev;
    }

    // IN THE CASE WHERE THE ALARM IS ARMED, THEN alarmState = 1
    // IN THE CASE WHERE THE ALARM IS DISSARMED,THEN  alarmState = 0
    switch(IrReceiver.decodedIRData.decodedRawData){
      // if the star button is pressed then turn the light on
      case BTN_ON:
        Serial.println("Alarm Armed");
        alarmState = 1; // set state to on
        alarmTriggered +=1;
        break;

      // if the pound button is pressed, then turn the light off
      case BTN_OFF:
        Serial.println("Alarm Off");
        alarmState = 0; // set state to off
        break;

      default:
        if(alarmTriggered >= 1){
          if(alarmState == 1)
            alarmState = 0;
          if(alarmState == 0)
            alarmState = 1;
        }
        break;
    }

    Serial.print("Alarm State: ");
    Serial.println(alarmState);
  }
 }


/**
 * This function will contain the logic for when the alarm is set.
 * In here you can find all the logic and functions that interact
 * with the system when the alarm is armed.
 * 
 * Author: Robert Ortiz
 * 2/25/21
 */
 void alarmIsSet(){
      if(alarmState == 1){
       lightAlarmOn(LED_RED); // blinks Red LED  to indicate the alarm system is on
      }
      else{
        digitalWrite(LED_RED,LOW);
      }
 }


/**
 * This function will be in charge of turning one of the LEDs on
 * to allow the user to know when the alarm is on or off.
 * 
 * @params LED : the led pin we wish to turn on
 * Author: Robert Ortiz
 */
void lightAlarmOn(int LED){
    digitalWrite(LED, HIGH);     // turn RED LED ON
    delay(100);                  // wait for 200ms
}
 /**
 * This function will blink the LEDS when the alarm is triggered
 * 
 * Author: Nga Le
 * 2/22/21
 */
void blinkLED(){
    digitalWrite(LED_BLUE, HIGH);    // turn on LED1 
    delay(100);                  // wait for 200ms
    digitalWrite(LED_GREEN, HIGH);    // turn on LED2
    delay(100);                  // wait for 200ms       
    digitalWrite(LED_RED, HIGH);    // turn on LED3 
    delay(100);                  // wait for 200ms
    digitalWrite(LED_BLUE, LOW);     // turn off LED1
    delay(200);                  // wait for 300ms
    digitalWrite(LED_GREEN, LOW);     // turn off LED2
    delay(200);                  // wait for 300ms
    digitalWrite(LED_RED, LOW);     // turn off LED3
    delay(200);                  // wait for 300ms before running program all over again
}

/**
 * This function is in charge of reading the from the magnetic sensor.
 * When the sensor switch is set to HIGH that means that the alarm has
 * been triggered by sepearting the magents. When it is in the LOW state,
 * the alarm is in tact. 
 * 
 * Author: Quyen Luu
 * 
 * @params sensor: The pin that our sensor is connected to
 */
void readFromMagnetSensor(int sensor){
  sensorState = digitalRead(sensor);
  if(sensorState == HIGH && alarmState >= 1){
    isAlarmTriggered = true;
  }
  if(isAlarmTriggered == true && alarmState != 0){
    buzz(BUZZER_PIN);
    blinkLED();
  }
  else{
    isAlarmTriggered = false;
    IrReceiver.resume(); // recieve next value
    readFromIrRemote();
  }
}

 /*
 * This function sounds the buzzer when the alarm is triggered. In this function
 * we utilize the included libraries like tone and noTone that allow us to controll
 * the Buzzer
 * 
 * Author: Joyce Pham
 */
void buzz(int BUZZZ) {
  tone(BUZZZ, 800);
  delay(300);
  noTone(BUZZZ);
  //delay(100);
}

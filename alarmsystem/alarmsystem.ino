#include <IRremote.h>

const int IR_RECEIVE_PIN = 10;

// EACH CONTROLLER MIGHT HAVE A DIFFERENT BUTTON LAYOUT
// WE NEED TO INDIVIDUALLY CHECK WHAT BUTTON TO USE
#define BTN_STAR 0XE916FF00
#define BTN_POUND 0xF20DFF00

// declare and init variables
const int LED_RED = 8; // USE a 100 ohm resistor
bool alarmState = false; // for handling the code when the alarm is on
long lastTime = millis(); // will help with keeping timing of our loops
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
  Serial.print("Enabling pin: ");
  Serial.println(IR_RECEIVE_PIN);

}

void loop() {
  long elapsedTime = millis() - lastTime; // get the elapsedTime
  lastTime = lastTime + elapsedTime; // update the last time
  readFromIrRemote();
  setAlarm(elapsedTime);
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
    // The remote by default will read zero so we should treat that as noise
    if(IrReceiver.decodedIRData.decodedRawData == 0xFFFFFFFF){
      IrReceiver.decodedIRData.decodedRawData = prev;
    }

    // IN THE CASE WHERE THE ALARM IS ARMED, THEN alarmState = 1
    // IN THE CASE WHERE THE ALARM IS DISSARMED,THEN  alarmState = 0
    switch(IrReceiver.decodedIRData.decodedRawData){
      // if the star button is pressed then turn the light on
      case BTN_STAR:
        Serial.println("Alarm Armed");
        alarmState = 1; // set state to on
        break;

      // if the pound button is pressed, then turn the light off
      case BTN_POUND:
        Serial.println("Alarm Off");
        alarmState = 0; // set state to off
        break;

      default:
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
 * @param elapsedTime : contains the last iteration of our setAlarm method
 * Author: Robert Ortiz
 * 2/25/21
 */
 void setAlarm(long elapsedTime){
    static long currTime = 0; 
    currTime += elapsedTime; 

    // this will allow our code to make time for other componenets
    if(currTime >= 1000){
      // make function calls for other components in here
      if(alarmState == 1){
        blinkLED(LED_RED); // blinks Red LED
      }
      else{
        digitalWrite(LED_RED,LOW);
      }
      currTime -= 1000; // reset time
    }    
 }


 /**
 * This function will blink the desired LED
   This function is a dummy function, I just needed to make 
 * something happen when testing the code.
 * 
 * @param LED: is the desired LED we wish to blink
 * 
 * Author: Robert Ortiz
 * 2/22/21
 */
void blinkLED(int LED){
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
}

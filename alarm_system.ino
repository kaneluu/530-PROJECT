#include <IRremote.h>

// EACH CONTROLLER MIGHT HAVE A DIFFERENT BUTTON LAYOUT
// WE NEED TO INDIVIDUALLY CHECK WHAT BUTTON TO USE
#define BTN_ON 0xFF6897
#define BTN_OFF 0xFF9867

const int TRIG_PIN = 12;
const int ECHO_PIN = 13;
const int BUZZER_PIN = 9;
const int IR_RECEIVE_PIN = 10;
const int LED_RED = 8;

const int MONITOR_DISTANCE = 50;

long lastTime = millis(); // will help with keeping timing of our loops
uint32_t prev; // will store a 32bit unsinged int value
//to measure the distance and time taken
long duration;
int distance;

bool alarmState = false; // for handling the code when the alarm is on

IRrecv ir_recv(IR_RECEIVE_PIN);
decode_results results;

/**
 * Code set up as usual
 * MAKE SURE TO PLACE SET UP CODE FOR ALL COMPONENTS HERE
 */
void setup()
{
  Serial.begin(9600);
  ir_recv.enableIRIn();
  
  pinMode(TRIG_PIN, OUTPUT); // Sets the TRIG_PIN as an OUTPUT
  pinMode(ECHO_PIN, INPUT); // Sets the ECHO_PIN as an INPUT
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT); // start the RED_LED
}


void loop()
{
  //long elapsedTime = millis() - lastTime; // get the elapsedTime
  //lastTime = lastTime + elapsedTime; // update the last time
  readFromIrRemote();
  //setAlarm(elapsedTime);
  
  // active the motion sensor when the alarm is armed
  if(alarmState) activeSensor();

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
  if (ir_recv.decode(&results))
  {
    // UNCOMMENT LINE BELOW TO PRINT REMOTE VALUES TO CONSOLE
    Serial.println(results.value, HEX);
    delay(200);
    // IN THE CASE WHERE THE ALARM IS ARMED, THEN alarmState = 1
    // IN THE CASE WHERE THE ALARM IS DISSARMED,THEN  alarmState = 0
    switch(results.value) {
      // if the star button is pressed then turn the light on
      case BTN_ON:
        Serial.println("Alarm Armed");
        alarmState = true;
        break;
      // if the pound button is pressed, then turn the light off
      case BTN_OFF:
        Serial.println("Alarm Off");
        alarmState = false;
        break;
      default:
        break;
    }
    
    Serial.print("Alarm State: ");
    Serial.println(alarmState);
    
    ir_recv.resume();
  }
 }

/**
 * This function will setup the motion sensor
 * 
 */
void activeSensor() {
  
  int sensorPinValue = 0;
  // Get sensor's trig pin value
  sensorPinValue = digitalRead(TRIG_PIN);
  // Sensor only be activate when its value is LOW
  if(sensorPinValue == 0) {
    // send out a sound wave with a speed of sound
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    // Calculate forward and bounce backward time of the echo
    duration = pulseIn(ECHO_PIN, HIGH);
    // Speed of sound wave divided by 2 (forward and bounce backward)
    // Speed of sound is 340 m/s or 0.034 cm/µs
    distance = duration * 0.034 / 2; 
    // Dectect on objects which are in range where define as MONITOR_DISTANCE
    if (distance >= MONITOR_DISTANCE) {
      digitalWrite(BUZZER_PIN,LOW); // make buzzer silent
      digitalWrite(LED_RED,LOW); // turn off the LED
    } else {
      buzz(BUZZER_PIN); // make buzzer buzzs
      blinkLED(LED_RED); // make LED blinks
    }
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
void blinkLED(int LED) {
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
}

/**
 * This function will buzz the desired buzzer
 * 
 * @param BUZZZ: is the desired buzzer we wish to buzz
 * 
 * Author: Joyce Pham
 * 03/06/21
 */
void buzz(int BUZZZ) {
  digitalWrite(BUZZZ, HIGH);
  delay(100);
  digitalWrite(BUZZZ, LOW);
  delay(100);
}

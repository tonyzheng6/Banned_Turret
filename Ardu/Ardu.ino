/**
 * Title:       Ardu.ino
 * Author:      Tony Zheng
 * Created on:  3/15/2015
 * Modified on: 5/19/2015
 * Description: Program for arduino that drives a mechanical servo based rubber band turret. 
 *              In order to correctly hit a target, the x axis's are manually controlled via
 *              Potentiometer whereas the y axis is calculated through a combination of sensors.
 *              Distance is sensed through an ultrasonic distance sensor. Height is then 
 *              calculated through a simple physics formula.
 * Notes:       Completed(A+ ✓)
 */
 
// Standard libraries.
#include <Servo.h>
#include <Wire.h>

// Pin numbers defined.
// Infrared motion sensor used to detect movement.
#define PIR_MOTION_SENSOR 2
// Continuous rotation servo used to launch rubber bands.
#define REVOLVER 5
// Regular servo used to turn the platform base.
#define ROTATOR 6
// Regular servo used to heighten/lower gun portion.
#define LEVER 4
// Potentiometer used to map location to the ROTATOR servo.
#define POTEN 5
// Toggle switch used to act as a armed and ready signal.
#define SWITCH 13
// Trigger pin used for ultrasonic distance sensor.
#define trigPin 8
// Echo pin used for ultrasonic distance sensor.
#define echoPin 9

// Global variables
int servoVal;
int switchState = LOW;
long loc;
Servo myRevolver;
Servo myTurret;
Servo myLever;

// Initial code thats run first when the arduino loads the program.
void setup() {
  // Delay is inserted as a safeguard to reprogram arduino.
  delay(1000); 
  // Begins a serial connection that is used for debugging through the serial monitor
  Serial.begin(9600);
  // Calls another function that sets the pins with the proper I/O
  pinsInit();
  // Attaches/Connects the servo for the turret base to the 'rotator' pin defined above
  myTurret.attach(ROTATOR);
}

// Initializing pins with proper I/O.
void pinsInit() {
  // PIR motion sensor set to input as it will send info based on what it detects
  pinMode(PIR_MOTION_SENSOR, INPUT);
  // Revolver servo set to output as it will receive an integer value to turn to that location.
  // This is the make-shift continuous rotation servo made from a regular servo, so it receives
  // an integer value and can be mapped so long as it is in a for-loop, otherwise it will turn
  // continuously at a constant rate. A real continuous servo has speed and direction of the servo 
  // dependent on the value given. 
  pinMode(REVOLVER, OUTPUT);
  // Rotator servo set to output as it will receive an integer value to turn to that location.
  pinMode(ROTATOR, OUTPUT);
  // Revolver servo set to output as it will receive an integer value to turn to that location.
  pinMode(LEVER, OUTPUT); 
  // Toggle switch set to input as it will will info based on the info it gives off.
  pinMode(SWITCH, INPUT); 
  // Trigger pin set to output as it will send an ultrasonic signal.
  pinMode(trigPin, OUTPUT);
  // Echo pin set to input as it will receive the ultrasonic signal sent back.
  pinMode(echoPin, INPUT);
}

// Code that loops continuously after the setup function. 
void loop() {
  // servoVal is given an analog value from the potentiometer pin.
  servoVal = analogRead(POTEN);
  // servoVal then gets the mapped value readable for writing to a servo.
  servoVal = map(servoVal, 0, 1023, 0, 180);    
  // myTurret gives the value of servoVal to myTurret so the platform turns based on where potentiometer is.
  myTurret.write(servoVal);   
  // switchState reads the value of the toggle switch (either on/off).
  switchState = digitalRead(SWITCH);
 
  // Serial print for the user to see the switch state, used just for testing purposes 
  Serial.println(switchState);
 
  // The toggle switch is used as a safety. If open, the turret doesn't operate.
  if(switchState == 0) {
    myLever.attach(LEVER);
    
    // loc is given the value returned by the react function.
    loc = react();
    
    // Only if loc is between 90 - 180 degrees (no need to shoot straight up or back at the user), 
    // it will write it's value to the servo of the gun portion.
    if(loc >= 90) {
      myLever.write(loc);
    }
    
    // Delays for 1 second so the PIR motion sensor stablizes before taking snapshots.
    delay(1000);
    
    // If a person is detected by the sensor and the safety switch is off, it fires.
    if(isPeopleDetected()) {
       // Calls function turnOnRevolver.
       turnOnRevolver();
    }
  }
  else {
    // Switch is open and acts as a safety	
  }
}

// Turns on the firing mechanism.
void turnOnRevolver() {
  // Attaches/connects the revolver servo.
  myRevolver.attach(REVOLVER);
  
  // For loop that tells the servo to turn to a location and a delay to slow down the rotation of the servo slightly.
  for(int i = 0; i < 90; i++) {
    myRevolver.write(i);
    delay(50);
  }
 
  // Detaches/disconnects the revolver servo else it will slowly turn continuously.
  myRevolver.detach();
}

// Turns off the firing mechanism.
void turnOffRevolver() {
  myRevolver.detach();
}

// Detects movement sensed. 
boolean isPeopleDetected() {  
  // sensorValue reads a value from the PIR motion sensor.
  // The PIR motion sensor takes a snapshot every 1-2 seconds and if it senses a change in the snapshots,
  // it sends either HIGH/LOW signal back.
  boolean sensorValue = digitalRead(PIR_MOTION_SENSOR);
  
  // If the sensorValue is HIGH, return true.
  if(sensorValue == HIGH) {
    return true;
  }
  // Else if the sensorValue is not HIGH, return false.
  else {
    return false;
  }
}

// Returns the angle position for the servo of the gun portion.
int react() {
  // Local variables declared that will get overwritten every loop iteration.
  long duration, distance, angle;
  // Writes LOW value to trigger pin.
  digitalWrite(trigPin, LOW);
  // Delays for 2 microseconds.
  delayMicroseconds(2);
  // Writes HIGH value to trigger pin to send a signal.
  digitalWrite(trigPin, HIGH);
  // Delays for 10 microseconds.
  delayMicroseconds(10);
  // Writes LOW value to trigger pin to turn off the signal.
  digitalWrite(trigPin, LOW);
  // duration is given the value it receives from the trigger pin, 
  // the duration of time between when the signal was sent to when it was received.
  duration = pulseIn(echoPin, HIGH);
  // The distance is calculated with this formula to convert it to distance from the distance sensor
  // and the target in front of it.
  distance = ((duration/2)/36.9);  
  // The angle is calculated to hit the midsection of the target (assuming a standing-up position) for the servo of the gun portion.
  angle =  atan(40/distance) * 180/3.14; 
  // Angle is 180 - angle because my servo was super-glued facing the other way.  
  angle = 180 - angle;
  // returns angle position for the servo.
  return angle;
}

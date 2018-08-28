#include <Arduino.h>
#include <Encoder.h>
#define CPI 720
#define SERIAL_BAUD 9600
#define DISPLAY_DELAY 10000
#define BUTTON_COUNT_THRESH 10
#define UPPER_LIMIT_PIN 10
#define LOWER_LIMIT_PIN 11
#define ENC_A_PIN 2
#define ENC_B_PIN 3
#define MOTOR_PWM_PIN 5
#define MOTOR_DIR_PIN 6
#define UP_BTN_PIN 12
#define DOWN_BTN_PIN 13
#define DEFAULT_SPEED 255
#define POSITION_TOLERANCE 50
#define MAX_VALUE -4.0
#define MIN_VALUE 5.0

//global variables
Encoder positionEncoder(ENC_A_PIN, ENC_B_PIN);

int displayDelayCounter = 0;
long currentPosition = -999;
long desiredPosition = 0;
int lowerLimitTriggered = 0;
int upperLimitTriggered = 0;
int motorDir = 0;
int motorSpeed =0;
int upButtonCounts = 0;
int downButtonCounts = 0;

void setup_pins(){
  pinMode(UPPER_LIMIT_PIN,INPUT_PULLUP);
  pinMode(LOWER_LIMIT_PIN,INPUT_PULLUP);
  //enc pins are handled with the Encoder object
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(MOTOR_DIR_PIN,OUTPUT);

  pinMode(UP_BTN_PIN,INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN,INPUT_PULLUP);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("Basic Encoder Test:");

  setup_pins();
}

void update_position(){
  currentPosition = positionEncoder.read();
};

void read_inputs(){
  upperLimitTriggered = digitalRead(UPPER_LIMIT_PIN);
  lowerLimitTriggered = digitalRead(LOWER_LIMIT_PIN);
  int u = digitalRead(UP_BTN_PIN);
  int d = digitalRead(DOWN_BTN_PIN);
  if ( u == 0 ){
    upButtonCounts += 1;
    if ( upButtonCounts > BUTTON_COUNT_THRESH){
      desiredPosition += 1;
      upButtonCounts = 0;
    }
    
  }
  if ( d == 0 ){
    downButtonCounts += 1;
    if ( downButtonCounts > BUTTON_COUNT_THRESH){
      desiredPosition -= 1;
      downButtonCounts = 0;
    }
    
  }  
};

void set_motor_output(){
  int positionError = currentPosition - desiredPosition;

  if ( abs(positionError) > POSITION_TOLERANCE){
   
    if ( positionError < 0 ){
       motorDir = 0;
       if ( lowerLimitTriggered == 0){
          motorSpeed = DEFAULT_SPEED;
       }
       else{
          motorSpeed = 0;
       }
    }
    else{
       motorDir = 1;
       if ( upperLimitTriggered == 0){
          motorSpeed = DEFAULT_SPEED;
       }
       else{
          motorSpeed = 0;
       }       
    }
  }
  else{
    //don't move
    motorSpeed = 0;
  }
  analogWrite(MOTOR_PWM_PIN,motorSpeed);
  digitalWrite(MOTOR_DIR_PIN,motorDir);
  
};

float toInches( long position ){
  return (float)position/(float)CPI;
};

void process_serial_command(){
  if ( Serial.available()){
    char command_char = Serial.read();

    switch(command_char){
      case 'h':
        desiredPosition -= 100;
        break;
      case 'k':
        desiredPosition += 100;
        break;
      case 'g':
        desiredPosition -= 5000;
        break;
      case 'l':
        desiredPosition += 5000;
        break;        
      case 'j':
        desiredPosition = 0;
        currentPosition =0;
        positionEncoder.write(0);
        break;
      default:
        Serial.print("Unrecognized command '");
        Serial.print(command_char);
        Serial.println("' : try again");
    }
  }
}

void update_display(){
  displayDelayCounter += 1;
  if ( displayDelayCounter > DISPLAY_DELAY){
    Serial.print("CURRENT=");
    Serial.print(toInches(currentPosition),4);
    Serial.print(",  DESIRED=");
    Serial.print(toInches(desiredPosition),4);    
    Serial.print(",  LIMITS=");
    Serial.print(lowerLimitTriggered);
    Serial.print(upperLimitTriggered);
    Serial.print(",  MOTOR::DIR=");
    Serial.print(motorDir);
    Serial.print(",SPD=");
    Serial.print(motorSpeed);    
    Serial.print("[H"); 
    displayDelayCounter = 0;
  }

};

void loop() {
  update_position();
  read_inputs();
  set_motor_output();
  update_display();
  process_serial_command();
}

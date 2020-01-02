#include "Arduino.h"
#include "Wire.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "OneButton.h"

//#include "ClickButton.h"
#include <ESP32Encoder.h>
#include "Ticker.h"
#define menuFont X11fixed7x14
#define CPI 360
#define SERIAL_BAUD 57600
#define DISPLAY_UPDATE_INTERVAL_MS 10
#define SERIAL_UPDATE_INTERVAL_MS 1000
#define BUTTON_PIN 27
#define ENC_A_PIN 25
#define ENC_B_PIN 26
#define I2C_ADDRESS 0x3C

void update_display();
void update_serial();
//global variables
//Encoder positionEncoder(ENC_A_PIN, ENC_B_PIN);
ESP32Encoder encoder;
SSD1306AsciiWire oled;
OneButton button(BUTTON_PIN,true);

Ticker displayTimer(update_display,DISPLAY_UPDATE_INTERVAL_MS,0,MILLIS);
Ticker serialTimer(update_serial,SERIAL_UPDATE_INTERVAL_MS,0,MILLIS);

volatile long currentPosition = -999;

void setupOLED(){
  Wire.setClock(400000L);  
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(menuFont);
  oled.displayRemap(true);
  oled.clear();

}

void button_SingleClick(){
  currentPosition = 0;
  encoder.setCount(0);
  Serial.println("clicked");
}
void button_DoubleClick(){
  currentPosition = 1000;
} 

void setupEncoder(){
  encoder.attachHalfQuad(ENC_A_PIN,ENC_B_PIN);
}

void setupButton(){
  button.attachDoubleClick(button_DoubleClick);
  button.attachClick(button_SingleClick);
  button.setDebounceTicks(20);
  button.reset();
  
}
void setup_pins(){
    //pinMode(BUTTON_PIN,INPUT_PULLUP);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin();
  displayTimer.start();
  serialTimer.start();
  setupOLED();
  setupEncoder();
  setup_pins();
  setupButton();
  Serial.println("Basic Encoder Test");
}

float toInches( long position ){
  return (float)position/(float)CPI;
};

void update_display(){
  oled.setCursor(0,0);
  oled.println("Readout 1.0");oled.clearToEOL();
  oled.print("P=");
  oled.print(toInches(currentPosition),4); oled.clearToEOL();
  oled.println(""); 
};

void update_serial(){
  Serial.print("P=");
  Serial.print(toInches(currentPosition),4);
  Serial.println("");   
}

void update_position(){
  currentPosition = encoder.getCount();
};

void loop() {  
  update_position();
  button.tick();
  displayTimer.update();
  serialTimer.update();
  //delay(5);
}

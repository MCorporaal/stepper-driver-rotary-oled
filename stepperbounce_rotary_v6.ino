
#include <AccelStepper.h>           // Gebruik AccelStepper bibliotheek
#include <Arduino.h>                // Gebruik Arduino biliotheek 
#include <U8g2lib.h>                // Gebruik oled biliotheek

// Zet variablen voor de stappenmotor
int stepsPerRevolution = 64;        // stappen per omwenteling
float degreePerRevolution = 5.625;    // graden per omwenteling

#define motorPin1  8                // IN1 pin op de ULN2003A driver
#define motorPin2  9                // IN2 pin op de ULN2003A driver
#define motorPin3  10               // IN3 pin op de ULN2003A driver
#define motorPin4  11               // IN4 pin op de ULN2003A driver
int r= 0;

AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

// Zet variablen voor de rotery encoder
int val;
int encoder0PinA = 3; // zet pin input voor pin A
int encoder0PinB = 4; // zet pin input voor pin B
int encoder0Pos = 0; // zet encoder naar 0
int encoder0PinALast = LOW; // zet encoder "er word nu niet aan gedraaid
int maxKnob = 200; // maximale voor de encoder
int n = LOW;

//zet variablen voor het oled display
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
unsigned long currentTime;
unsigned long loopTime;

//U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
 
  Serial.begin(9600);

// Encoder setup
  pinMode (encoder0PinA, INPUT); //Vraagt om input encoder pin A
  pinMode (encoder0PinB, INPUT); //Vraagt om input encoder pin B
 
// Stepper setup
  stepper.setMaxSpeed(1000);
  //Deze waardes finetunen
  stepper.setAcceleration(800);
  stepper.moveTo(32000);

// Oled setup
  u8g2.begin();
  u8g2.setFont(u8g2_font_sirclive_tr);  // Zet een font voor het oled scherm
  currentTime = millis();
  loopTime = currentTime;  
}
void loop() {
    motorStep();
    getKnob();
    displayOnOled();
}

// 5 volt stepper program
void motorStep(){
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());
    stepper.run();
    
}

// Vraag encoder plek
void getKnob(){
     n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) { 
    if (digitalRead(encoder0PinB) == LOW) { //Als pinB eerst laag word doe - 1
      encoder0Pos--;
    } else { //Anders doe + 1
      encoder0Pos++;
    }
    if( encoder0Pos > maxKnob ){ //is de encoder waarde grooter dan de max zet hem terug naar max
         encoder0Pos = maxKnob;
    }
    else if( encoder0Pos < 0 ){ //is de encoder waarde kleiner dan 0 zet hem terug naar 0
         encoder0Pos = 0;
    } 

     r = encoder0Pos * 5;
       
     stepper.setMaxSpeed(r); // zet de stappen moter snelheid

     Serial.println ( r );
    }
  encoder0PinALast = n;
}

void displayOnOled() {
  currentTime = millis();
  if(currentTime >= (loopTime + 1000)){ // Zet refresh time
    u8g2.clearBuffer();          // Gooi display memory leeg
   u8g2.drawFrame(0, 0, 128, 64);
   u8g2.setCursor(4, 10);
   u8g2.print("CAMERA SLIDER");
    u8g2.setCursor(2, 30);
    u8g2.print("encoder: ");
    u8g2.setCursor(80, 30);
    u8g2.print(r);
    u8g2.setCursor(2, 55);
    u8g2.print("position: ");
    u8g2.setCursor(80, 55);
    u8g2.print(stepper.currentPosition());
    u8g2.sendBuffer();          // Verstuur naar de display
    loopTime = currentTime;  // Update loopTime
  }
}
 

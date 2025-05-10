#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <SPI.h>

// define the buttons pins
const int buttonStart_pin = 2;
const int buttonReset_pin = 3;
const int buttonStop_pin = 4;
// define route pin  
const int THIS_ROUTE = 9 ; 

// define TM1637 pins
const int CLK_PIN = 13;
const int DIO_PIN = 8;

// define wait_timer
const uint16_t WAIT_DELAY = 1000;

// define traffic light pins
#define Light_RedPin 5
#define Light_YellowPin 6
#define Light_GreenPin 7

// define the MAX72xx pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_pin 13
#define DATA_pin 11
#define CS_pin 8

// define the buzzer
const int THIS_Buzzer = 12;

MD_Parola myParolaMD = MD_Parola(HARDWARE_TYPE, CS_pin, MAX_DEVICES);
int errors = 0;
bool countingDown = false;
unsigned long startTime = 0;
const unsigned long countdownDuration = 59 * 60 + 59;
TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {

  myParolaMD.begin();
  display.setBrightness(7);
  // INPUT DEVICES
  pinMode(buttonStart_pin, INPUT_PULLUP);
  pinMode(buttonReset_pin, INPUT_PULLUP);
  pinMode(buttonStop_pin, INPUT_PULLUP);
  pinMode(THIS_ROUTE, INPUT_PULLUP);

  // OUTPUT DEVICES
  pinMode(Light_GreenPin, OUTPUT);
  pinMode(Light_YellowPin, OUTPUT);
  pinMode(Light_RedPin, OUTPUT);
  pinMode(THIS_Buzzer, OUTPUT);
  noTone(THIS_Buzzer);
}

void loop() {

  int button = 0;
  int buttonState_Start = digitalRead(buttonStart_pin);
  int buttonState_Reset = digitalRead(buttonReset_pin);
  int buttonState_Stop = digitalRead(buttonStop_pin);

  // Daca oricare din butoane este apasat inseamna ca este low.
  if (buttonState_Start == LOW ) {
    tone(THIS_Buzzer, 1000) ;
    delay(1000);
    countingDown = true;
    startTime = millis();
    button = 1;
  }else {
    noTone(THIS_Buzzer);
    button = 1 ;
  }

  if (buttonState_Stop == LOW) {
    tone(THIS_Buzzer, 1000) ;
    delay(1000);
    countingDown = false;
    button = 2;
  }else {
    noTone(THIS_Buzzer);
  }

  if (buttonState_Reset == LOW) {
    tone(THIS_Buzzer, 1000) ;
    delay(1000);
    countingDown = false;
    display.showNumberDec(5959);
    button = 3;
  }else {
    noTone(THIS_Buzzer);
  }

  switch (button) {
    case 1:
      buttonStartPressed_Execute();
      break;
    case 2:
      buttonStopPressed_Execute();
      break;
    case 3:
      buttonResetPressed_Execute();
      break;
    default:
      break;
  }

  if (countingDown) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = (currentTime - startTime) / 1000;  // Converteste în secunde

    if (elapsedTime >= countdownDuration) {
      // Numărătoarea inversă s-a încheiat
      countingDown = false;
      display.showNumberDec(0);  // Afișează zero pe modulele TM1637
    } else {
      unsigned long remainingTime = countdownDuration - elapsedTime;
      unsigned int minutes = remainingTime / 60;
      unsigned int seconds = remainingTime % 60;

      display.showNumberDecEx(minutes * 100 + seconds, 0b01000000, true);  // Afișează minutele și secundele
    }
  }

}

void buttonStartPressed_Execute() {
  
  Serial.begin(9600);
  int ROUTE_State = digitalRead(THIS_ROUTE);
  if (ROUTE_State == LOW) {  // deci are contact.
    countErrorsByTouching ( ) ;
    Serial.println(errors) ;
  }else{

  }

  myParolaMD.print("Erori:");
  delay(WAIT_DELAY);
  myParolaMD.print(errors);

  if (errors >= 0 && errors <= 5) {
    digitalWrite(Light_GreenPin, HIGH);
    digitalWrite(Light_YellowPin, LOW);
    digitalWrite(Light_RedPin, LOW);

  } else if (errors >= 5 && errors <= 10) {
    digitalWrite(Light_GreenPin, LOW);
    digitalWrite(Light_YellowPin, HIGH);
    digitalWrite(Light_RedPin, LOW);

  } else if (errors >= 10 && errors <= 15) {
    digitalWrite(Light_GreenPin, LOW);
    digitalWrite(Light_YellowPin, LOW);
    digitalWrite(Light_RedPin, HIGH);
  }

}

void buttonStopPressed_Execute() {

  myParolaMD.displaySuspend(true);
  if (errors >= 0 && errors <= 5) {
    digitalWrite(Light_GreenPin, HIGH);
    digitalWrite(Light_YellowPin, LOW);
    digitalWrite(Light_RedPin, LOW);

  } else if (errors >= 5 && errors <= 10) {
    digitalWrite(Light_GreenPin, LOW);
    digitalWrite(Light_YellowPin, HIGH);
    digitalWrite(Light_RedPin, LOW);

  } else if (errors >= 10 && errors <= 15) {
    digitalWrite(Light_GreenPin, LOW);
    digitalWrite(Light_YellowPin, LOW);
    digitalWrite(Light_RedPin, HIGH);
  }

  myParolaMD.print(errors);

}

void buttonResetPressed_Execute() {

  myParolaMD.displayReset();
  errors = 0;
  digitalWrite(Light_GreenPin, LOW);
  digitalWrite(Light_YellowPin, LOW);
  digitalWrite(Light_RedPin, LOW);

}

void countErrorsByTouching ( ) {
  errors++ ;
}
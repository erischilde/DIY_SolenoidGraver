/*
have two options for getting analog input, normal method is default. 
if want to try with delays, move it to checkbuttons code. 
*/



#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 20, 4);


//PWN Pins avvailable 3, 5, 6, 9, 10, 11.

// Pins
const int FrequencyPin = A0;
const int DutyPin = A1;
const int PedalPin = A2;
const int EN_A_B = 3;
const int IN1_3 = 11;
const int IN2_4 = 9;

const int Button1 = 4; // Cycle Button
const int Button2 = 7; // Standby Button

// Operating Modes
enum Mode { STANDBY, ENGRAVE, MODE_COUNT };
Mode currentMode = STANDBY;

// Debounce & State Variables for Button 1
int lastB1State = LOW;
int confirmedB1State = LOW;
unsigned long lastB1DebounceTime = 0;

// Debounce & State Variables for Button 2
int lastB2State = LOW;
int confirmedB2State = LOW;
unsigned long lastB2DebounceTime = 0;

const unsigned long debounceDelay = 50; 

unsigned long PreviousTime = 0;
unsigned long previousLCDMillis = 0;
const long lcdInterval = 150; 

//Analog Input Variables
  int rawFreq  = 0;
  int rawDuty  = 0;
  int rawPedal = 0;
// Delay variables for Potentiometer reads
unsigned long potcheck = 0;


void setup() {
  pinMode(FrequencyPin, INPUT); 
  pinMode(DutyPin, INPUT);
  pinMode(PedalPin, INPUT);
  pinMode(Button1, INPUT); 
  pinMode(Button2, INPUT); 
  pinMode(EN_A_B, OUTPUT);
  pinMode(IN1_3, OUTPUT);
  pinMode(IN2_4, OUTPUT);
  
  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  Serial.begin(115200);
}

void loop() {
  checkButtons();
  rawDuty = analogRead(DutyPin);
  rawFreq = analogRead(FrequencyPin);
  rawPedal = analogRead(PedalPin);
  updateLCD(rawFreq, rawDuty, rawPedal);
  
  switch (currentMode) {
    case STANDBY:     doStandby();                             break;
    case ENGRAVE:     doEngraving(rawFreq, rawDuty, rawPedal); break; 
  }
}

void checkButtons() {
  // --- BUTTON 1 LOGIC (CYCLE) ---
  int reading1 = digitalRead(Button1);
  if (reading1 != lastB1State) {
    lastB1DebounceTime = millis();
  }
  if ((millis() - lastB1DebounceTime) > debounceDelay) {
    if (reading1 != confirmedB1State) {
      confirmedB1State = reading1;
      if (confirmedB1State == HIGH) {
        //currentMode = (currentMode == ENGRAVE) ? STANDBY : (Mode)(currentMode + 1);
        currentMode = (Mode)((currentMode + 1) % MODE_COUNT);
        Serial.println(currentMode);
        lcd.clear();
      }
    }
  }
  lastB1State = reading1;

  // --- BUTTON 2 LOGIC (FORCE STANDBY) ---
  int reading2 = digitalRead(Button2);
  if (reading2 != lastB2State) {
    lastB2DebounceTime = millis();
  }
  if ((millis() - lastB2DebounceTime) > debounceDelay) {
    if (reading2 != confirmedB2State) {
      confirmedB2State = reading2;
      if (confirmedB2State == HIGH) {
        currentMode = STANDBY; // Force to Standby
        lcd.clear();
        Serial.println("Reset to Standby");
      }
    }
  }
  lastB2State = reading2;
  //reading analog inputs
/*
  potcheck = millis();
  if ((millis() - potcheck) > 50 ){
    rawDuty = analogRead(DutyPin);
  }
  if ((millis() - potcheck) > 100 ){
    rawFreq = analogRead(FrequencyPin);
  }
  if ((millis() - potcheck) > 150 ){
    rawPedal = analogRead(PedalPin);
  }
*/
}

// --- H-BRIDGE BIDIRECTIONAL CONTROL FUNCTIONS ---
void fireStroke(int pwm) {
  digitalWrite(IN1_3, HIGH);
  digitalWrite(IN2_4, LOW);
  analogWrite(EN_A_B, pwm);
}

void fireReturn(int pwm) {
  digitalWrite(IN1_3, LOW);
  digitalWrite(IN2_4, HIGH);
  analogWrite(EN_A_B, pwm);
}

void stopSolenoid() {
  digitalWrite(IN1_3, LOW);
  digitalWrite(IN2_4, LOW);
  analogWrite(EN_A_B, 0);
}

void doEngraving(int rawFreq, int rawDuty, int rawPedal) {
//  Serial.println("Engrave");
  float Frequency = map(rawFreq, 0, 1023, 10, 60);
  float Duty = map(rawDuty, 0, 1023, 5, 25) / 100.0;
  float Period = 1000.0 / Frequency;
  //Serial.println(Frequency);
  //Serial.println(Duty);
  if (rawPedal > 50) {
    int pwm = map(rawPedal, 0, 1023, 0, 255);
    if (pwm > 218) pwm = 255;
    unsigned long timeInCycle = millis() - PreviousTime;
    float pulsewidth = Period * Duty;
    float breaker = pulsewidth + 5;
    float ender = (pulsewidth*2)+5;
    if (timeInCycle < pulsewidth) {
      fireStroke(pwm);
    }
    else if (timeInCycle < breaker) {
      stopSolenoid();
    } 
    else if (timeInCycle < ender) {
      fireReturn(200);
    }
    else {
      stopSolenoid();
    }
    if (timeInCycle >= Period) PreviousTime = millis();
  } else {
    stopSolenoid();
  }
}

void doStandby() { 
  stopSolenoid(); 
}

void updateLCD(int rawFreq, int rawDuty, int rawPedal) {
  if (millis() - previousLCDMillis < lcdInterval) return;
  previousLCDMillis = millis();
  lcd.setCursor(0, 0);
  lcd.print("MODE: ");
  switch (currentMode) {
    case STANDBY:     lcd.print("STANDBY "); break;
    case ENGRAVE:     lcd.print("ENGRAVE "); break;
  }
  lcd.setCursor(0, 1);
  lcd.print("F:"); lcd.print(map(rawFreq, 0, 1023, 2, 60));
  lcd.print(" D:"); lcd.print(map(rawDuty, 0, 1023, 1, 30));
  lcd.print("%   ");
  lcd.setCursor(0, 3);
  lcd.print("B1:Cycle B2:Stop");
}

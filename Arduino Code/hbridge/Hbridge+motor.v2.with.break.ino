//this version includes a 5ms "break" between fire and retract to allow magnetic field to collapse. 
//this is optimized for 20 to 40hz, but you left it at 2-60hz, so if there's issues with timing, check there. 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);


//PWN Pins avvailable 3, 5, 6, 9, 10, 11.

// Pins
const int FrequencyPin = A0;
const int DutyPin = A1;
const int PedalPin = A2;
const int ENA = 3;
const int MosfetPin = 10;
const int extend = 11;
const int retract = 9;

const int Button1 = 4; // Cycle Button
const int Button2 = 7; // Standby Button

// Operating Modes
enum Mode { STANDBY, ENGRAVE, MOTOR, SINGLE_SHOT };
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

void setup() {
  pinMode(Button1, INPUT); 
  pinMode(Button2, INPUT); 
  pinMode(MosfetPin, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(extend, OUTPUT);
  pinMode(retract, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
}

void loop() {
  int rawFreq  = analogRead(FrequencyPin);
  int rawDuty  = analogRead(DutyPin);
  int rawMax   = analogRead(DutyPin);
  int rawPedal = analogRead(PedalPin);

  checkButtons();   
  updateLCD(rawFreq, rawDuty, rawMax, rawPedal);
  
  switch (currentMode) {
    case STANDBY:     doStandby();                             break;
    case ENGRAVE:     doEngraving(rawFreq, rawDuty, rawPedal); break; 
    case MOTOR:       doMotor(rawMax, rawPedal);               break; 
    case SINGLE_SHOT: doSingleShot(rawPedal);                  break;
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
        currentMode = (currentMode == SINGLE_SHOT) ? STANDBY : (Mode)(currentMode + 1);
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
}


void doMotor(int rawMax, int rawPedal) {
  int maxpwm = map(rawMax, 0, 1023, 0, 255);
  int PWMout = map(rawPedal, 0, 1023, 0, maxpwm);
  if (PWMout >= 218) PWMout = 255;
  if (rawPedal > 50) {
    analogWrite(MosfetPin, PWMout);
  } else {
    analogWrite(MosfetPin, 0);
  }
}

void doEngraving(int rawFreq, int rawDuty, int rawPedal) {
  analogWrite(MosfetPin, 0);
  float Frequency = map(rawFreq, 0, 1023, 60, 2); //code this is taken from uses 20-40 hz, so if timing is off, maybe adjust this. 
  float Duty = map(rawDuty, 0, 1023, 5, 25) / 100.0;
  float Period = 1000.0 / Frequency;
  
  if (rawPedal > 50) {
    int pwm = map(rawPedal, 0, 1023, 0, 255);
    if (pwm > 218) pwm = 255;
    unsigned long timeInCycle = millis() - PreviousTime;
    float pulseWidth = Period * Duty;

    if (timeInCycle < pulseWidth ) {
      analogWrite(ENA, pwm);
      digitalWrite(extend,HIGH);
      digitalWrite(retract,LOW);
    } 
    else if (timeInCycle < pulseWidth + 5) {
      analogWrite(ENA, 0);
      digitalWrite(extend,LOW);
      digitalWrite(retract,LOW);
    } 
    else if ( timeInCycle < (Period * 0.8)){
      analogWrite(ENA, 200);
      digitalWrite(extend,LOW);
      digitalWrite(retract,HIGH);
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
  analogWrite(MosfetPin, 0); 
}

void doSingleShot(int rawPedal) {
  analogWrite(MosfetPin, 0);
  static bool fired = false;
  if (rawPedal > 100 && !fired) {
    analogWrite(ENA, 255);
    digitalWrite(extend,HIGH);
    digitalWrite(retract,LOW);
    delay(25); 
    digitalWrite(extend,LOW);
    digitalWrite(retract,HIGH);
    stopSolenoid();
    fired = true;
  } else if (rawPedal < 60) {
    fired = false;
  }
}

void stopSolenoid() {
  analogWrite(ENA, 0);
}

void updateLCD(int rawFreq, int rawDuty, int rawMax, int rawPedal) {
  if (millis() - previousLCDMillis < lcdInterval) return;
  previousLCDMillis = millis();

  lcd.setCursor(0, 0);
  lcd.print("MODE: ");
  switch (currentMode) {
    case STANDBY:     lcd.print("STANDBY "); break;
    case ENGRAVE:     lcd.print("ENGRAVE "); break;
    case MOTOR:       lcd.print("MOTOR   "); break;
    case SINGLE_SHOT: lcd.print("SINGLE-S"); break;
  }
  
  if (currentMode = MOTOR){
    lcd.setCursor(0, 2);
    lcd.print("MaxPWM: "); lcd.print(map(rawMax, 0, 1023, 0, 255));
    lcd.setCursor(0, 1);
    lcd.print("Speed:"); lcd.print(map(rawPedal, 0, 218, 0, 100));
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("F:"); lcd.print(map(rawFreq, 0, 1023, 60, 2));
    lcd.print(" D:"); lcd.print(map(rawDuty, 0, 1023, 30, 1));
    lcd.print("%   ");
  }
  lcd.setCursor(0, 3);
  lcd.print("B1:Cycle B2:Stop");
}

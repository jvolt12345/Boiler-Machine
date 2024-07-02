#include "max6675.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int relayPin = 7;   //  relay pin
int arduino = 0;    //  represents the value of motor: 0 if on and 1 if off

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

int r = 0;      // lcd reset variable
int x = 0;      // lcd reset variable
int y = 0;      // starting time of pasteurization
int z = 0;      // lcd reset variable

int pb = 3;


MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int secondTime;     // represents the time in seconds
int minuteTime;     // represents the time in minutes
int hourTime;       // represents the time in hours

void setup() {
  lcd.init();       // initialize lcd  
  lcd.backlight();  // turn on backlight

  pinMode(relayPin, OUTPUT);    // relaypin
  digitalWrite(relayPin, HIGH); // Turn on the relay initially (assuming LOW is for turning on the motor)
}

void loop() {
 seconds();
}

void seconds(){
  
  while (secondTime <= 58){
    secondTime++;
    temp();

    if (digitalRead(pb) == LOW){
      lcd.init();
      lcd.clear();
    }

    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(thermocouple.readCelsius());
    lcd.print((char)223);           //degree symbol
    lcd.print("c");
    
    lcd.setCursor(11,1);
    lcd.print(secondTime);
    lcd.print("sec");
   
    lcd.setCursor(5,1);
    lcd.print(minuteTime);
    lcd.print("min");

    lcd.setCursor(0,1);
    lcd.print(hourTime);
    lcd.print("hr");
    
      delay(1000);
    }
    
  if (secondTime >= 58){
      secondTime = -1; 
      minuteTime++;
      if (minuteTime >= 60){
        minuteTime = 0;   
        hourTime++;   
      }
   }
}

void temp(){
  // Read temperature from the thermocouple sensor
  double temperature = thermocouple.readCelsius();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Check if temperature is above 100°C and motor is on
  if (temperature >= 100 && arduino == 0) {
    digitalWrite(relayPin, LOW); // Turn off the motor
    
      if (x < 2){     // wait for 2 seconds to reset lcd
        z = 0;
        x = x + 1;
        lcd.begin(16,2);    //  resets lcd
        lcd.clear();        //  resets lcd
      }
  }

  // Check if temperature is below 90°C and motor is off
  else if (temperature <=90 && arduino == 0) {
    digitalWrite(relayPin, HIGH); // Turn on the motor
    
      if (z < 2){     // wait for 2 seconds to reset lcd
        x = 0;
        z = z + 1;
        lcd.begin(16,2);
        lcd.clear();
      }
  }

  // Check if 6 hours have elapsed
  if (hourTime >= 6) {      // 6 hours pasteurization time reached: will turn off the motor
    arduino = 1;          // represents the value of motor turned off
    digitalWrite(relayPin, LOW);

    if (r < 2){       // wait for 2 seconds to reset lcd
      r = r + 1;
      lcd.begin(16,2);
      lcd.clear();
    }
  }

  //  the pasteurization timer starts at 80°c
  if (temperature >= 80) {      //  starting time of pasteurization
      y = y +  1;
  }
  if (y == 1){        //  resets the time
      secondTime = 0;
      minuteTime = 0;
      hourTime = 0;
    }

}

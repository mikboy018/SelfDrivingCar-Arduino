
// Thermistor - https://www.sunfounder.com/thermistor-sensor-module.html
// Thermistor Code adapted from - https://www.sunfounder.com/learn/universal-kit-for-Arduino/lesson-8-thermistor-universal-kit.html
// Basic code from - http://www.circuitbasics.com/how-to-set-up-an-ultrasonic-range-finder-on-an-arduino/
#include <LiquidCrystal_I2C.h>
#include <math.h>
#define trigPinR 9
#define trigPinC 10
#define trigPinL 11
#define echoPinR 2
#define echoPinC 3
#define echoPinL 4
#define analogPin A0 //the thermistor attach to 
#define beta 4090 //the beta of the thermistor
#define resistance 5100 //the value of the pull-down resistor

double Thermistor(int RawADC) {
 double Temp;
 Temp = log(10000.0*((1024.0/RawADC-1))); 
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;          
 return Temp;
}

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin (9600);
  pinMode(trigPinR, OUTPUT);
  pinMode(trigPinC, OUTPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(echoPinC, INPUT);
  pinMode(echoPinL, INPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int val;                
  double temp; 
  String dir = "";           
  val=analogRead(0);      
  temp=Thermistor(val);
  
  float durationR, durationC, durationL, distanceR, distanceC, distanceL;
  digitalWrite(trigPinR, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);
  durationR = pulseIn(echoPinR, HIGH);
  Serial.println(String(durationR) + " (R)");
  digitalWrite(trigPinC, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPinC, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinC, LOW);
  durationC = pulseIn(echoPinC, HIGH);
  Serial.println(String(durationC) + " (C)");
  digitalWrite(trigPinL, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);
  durationL = pulseIn(echoPinL, HIGH);
  Serial.println(String(durationL) + " (L)");
  float spdSnd = 331.4 + (0.606 * temp) + 0.62;
  distanceR = (durationR / 2) * (spdSnd / 10000);
  distanceC = (durationC / 2) * (spdSnd / 10000);
  distanceL = (durationL / 2) * (spdSnd / 10000);
  Serial.println(String(distanceL) + " / " + String(distanceC) + " / " + String(distanceR));
  //read humiture value 
  long a = analogRead(analogPin);
  //the calculating formula of temperature
  float tempC = beta /(log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
  //float tempF = 1.8*tempC + 32.0;//convert centigrade to Fahrenheit
  Serial.print("TempC: ");//print" TempC: "
  Serial.print(tempC);//print Celsius temperature
  Serial.print(" C");//print the unit
  Serial.println();
  //Serial.print("TempF: ");
  // Serial.print(tempF);
  // Serial.print(" F");
/*
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println("Out of range");
    Serial.println(distance);
  }
  else {
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" cm");
    delay(500);
  }
  delay(500);*/

 //lcd.print(distanceL + "/" + distanceC + "/" + distanceR);
 if(distanceL >= 400 || distanceL == 0)
  distanceL = 400;
 if(distanceC >= 400 || distanceC == 0)
  distanceC = 400;
 if(distanceR >= 400 || distanceR == 0)
  distanceR = 400;

 if(distanceL >= distanceR)
 {
  if(distanceC >= distanceL)
    dir = "FWD";
  else
    dir = "LFT";
 }
 else
 {
  if(distanceC >= distanceR)
    dir = "FWD";
  else
    dir = "RGT";
 }
  lcd.setCursor(0,0);
  lcd.print(String("Dir - ") + dir);
  lcd.setCursor(0,1);
  lcd.print(String(round(distanceL)) + "/" + String(round(distanceC)) + "/" + String(round(distanceR)));
 
}


// Thermistor - https://www.sunfounder.com/thermistor-sensor-module.html
// Thermistor Code adapted from - https://www.sunfounder.com/learn/universal-kit-for-Arduino/lesson-8-thermistor-universal-kit.html
// Basic code from - http://www.circuitbasics.com/how-to-set-up-an-ultrasonic-range-finder-on-an-arduino/
// Motor control examples - https://howtomechatronics.com/tutorials/arduino/arduino-dc-motor-control-tutorial-l298n-pwm-h-bridge/
#include <LiquidCrystal_I2C.h>
#include <math.h>
#define TrigL 2
#define EchoL 3
#define TrigC 4
#define EchoC 5
#define TrigR 7
#define EchoR 6
#define analogPin A0 //the thermistor attach to 
#define beta 4090 //the beta of the thermistor
#define resistance 5100 //the value of the pull-down resistor
#define EnA 10
#define In1 8
#define In2 9
#define EnB 11
#define In3 12
#define In4 13

int motorSpeedHigh = 90;
int motorSpeedLow = 75;
String dir = "FWD";

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
  pinMode(TrigL, OUTPUT);
  pinMode(EchoL, INPUT);
  pinMode(TrigC, OUTPUT);
  pinMode(EchoC, INPUT);
  pinMode(TrigR, OUTPUT);
  pinMode(EchoR, INPUT);
  pinMode(EnA, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(EnB, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(500);
  int val;                
  double temp; 
  String dir = "";           
  val=analogRead(0);      
  temp=Thermistor(val);
  
  float durationR, durationC, durationL, distanceR, distanceC, distanceL;
  digitalWrite(TrigR, LOW); 
  delayMicroseconds(2);
  digitalWrite(TrigR, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigR, LOW);
  durationR = pulseIn(EchoR, HIGH);
  Serial.println(String(durationR) + " (R)");
  digitalWrite(TrigC, LOW); 
  delayMicroseconds(2);
  digitalWrite(TrigC, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigC, LOW);
  durationC = pulseIn(EchoC, HIGH);
  Serial.println(String(durationC) + " (C)");
  digitalWrite(TrigL, LOW); 
  delayMicroseconds(2);
  digitalWrite(TrigL, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigL, LOW);
  durationL = pulseIn(EchoL, HIGH);
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
 if(distanceL > 400)
 { 
  distanceL = 400;
 }
 else if (distanceL <= 10)
 {
  distanceL = 0;
 }
 
 if(distanceC > 400)
 {
  distanceC = 400;
 }
 else if (distanceC <= 10)
 {
  distanceC = 0;
 }
 
 if(distanceR > 400)
 {
  distanceR = 400;
 }
 else if (distanceR <= 10)
 {
  distanceR = 0;
 }

 if (distanceC < 200)
 {
  if( distanceC > 0 && distanceR > 0 && distanceL > 0)
  {
    if(distanceL >= distanceR)
    {
      if(distanceC >= distanceL)
      {
        dir = "FWD";
      }
      else
      {
        dir = "LFT";
      }
    
    } 
    else
    {
      if(distanceC >= distanceR)
      { 
        dir = "FWD";
      }
      else
      {
        dir = "RGT";
      }
    }
  } 
  else
  {
    dir = "BCK";
  }
 }
 else
 {
  dir = "FWD";
 }
 
 lcd.setCursor(0,0);
 lcd.print(String("Chose - ") + dir);
 lcd.setCursor(0,1);
 lcd.print(String(round(distanceL)) + "/" + String(round(distanceC)) + "/" + String(round(distanceR)) + "    ");

 if(dir == "FWD")
 {
  Serial.println("Moving forward");

  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  analogWrite(EnA, motorSpeedHigh);
  analogWrite(EnB, motorSpeedHigh);
 }
 else if(dir == "RGT")
 {
  Serial.println("Turning Right");

  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  analogWrite(EnA, motorSpeedLow);
  analogWrite(EnB, motorSpeedLow);
 }
  else if(dir == "LFT")
 {
  Serial.println("Turning Left");

  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  analogWrite(EnA, motorSpeedLow);
  analogWrite(EnB, motorSpeedLow);
 }
 else
 {
  Serial.println("Backing Up");

  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  analogWrite(EnA, motorSpeedLow);
  analogWrite(EnB, motorSpeedLow);
 }
 
}

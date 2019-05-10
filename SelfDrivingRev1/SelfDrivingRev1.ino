// Thermistor - https://www.sunfounder.com/thermistor-sensor-module.html
// Thermistor Code adapted from - https://www.sunfounder.com/learn/universal-kit-for-Arduino/lesson-8-thermistor-universal-kit.html
// Basic code from - http://www.circuitbasics.com/how-to-set-up-an-ultrasonic-range-finder-on-an-arduino/
// Motor control examples - https://howtomechatronics.com/tutorials/arduino/arduino-dc-motor-control-tutorial-l298n-pwm-h-bridge/
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Trigger and Echo pins for Left/Center/Right Ultrasonic Range Finders
#define TrigL 2
#define EchoL 3
#define TrigC 4
#define EchoC 5
#define TrigR 7
#define EchoR 6
// Thermistor variables 
#define analogPin A0 //the thermistor attach to 
#define beta 4090 //the beta of the thermistor
#define resistance 5100 //the value of the pull-down resistor
// Motor control pins for each wheel controlled by Dual H-Bridge
#define EnA 10
#define In1 8
#define In2 9
#define EnB 11
#define In3 12
#define In4 13

// Motor Variables
int motorSpeedHigh = 100;
int motorSpeedLow = 80;
// Direction chosen based on comparison of three ranges
String dir = "FWD";

// LCD Setup for readout of sensed range data for each ultrasonic range finder and chosen direction.
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
  double tempC;
  double sndSpd; 
  double durationR, durationC, durationL, distanceR, distanceC, distanceL;       

  //read thermistor value 
  long a = analogRead(analogPin);
  //the calculating formula of temperature, with an estimate of humidity
  tempC = beta /(log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
  sndSpd = 331.4 + (0.606 * tempC) + 0.62;

  // check Left, Center, and Right range finders for a distance
  distanceL = sensorDuration(TrigL, EchoL, sndSpd, 'L');
  distanceC = sensorDuration(TrigC, EchoC, sndSpd, 'C');
  distanceR = sensorDuration(TrigR, EchoR, sndSpd, 'R');
  
  Serial.println(String(distanceL) + " / " + String(distanceC) + " / " + String(distanceR));
  Serial.print("TempC: ");//print" TempC: "
  Serial.print(tempC);//print Celsius temperature
  Serial.print(" C");//print the unit
  Serial.println();

  determineDirection(distanceL, distanceC, distanceR);
  drive();

  printDirection(distanceL, distanceC, distanceR);
}

// checks Ultrasonic Sensor based on specified pins, returns duration betweent trigger and echo
double sensorDuration(int trigPin, int echoPin, double sndSpd, char dir)
{
  double duration;
  double distance;

  // send trigger
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // receive echo to measure duration
  duration = pulseIn(echoPin, HIGH);
  Serial.println(String(duration) + " " + dir);

  // calculate distance & return value (max reliable range 400 cm)
  distance = (duration / 2) * (sndSpd / 10000);
  if(distance > 400.00)
    distance = 400.00;
  else if (distance <= 10.00)
    distance = 0.00;
    
  return distance;
}

// compares left, center, and right distances for determining direction
void determineDirection(double distanceL, double distanceC, double distanceR)
{
 if (distanceC < 75)
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
}

// based on chosen direction, applies signals to spin the right and left motor
void drive()
{
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
    analogWrite(EnB, motorSpeedHigh);
  }
  else if(dir == "LFT")
  {
    Serial.println("Turning Left");

    digitalWrite(In1, HIGH);
    digitalWrite(In2, LOW);
    digitalWrite(In3, LOW);
    digitalWrite(In4, HIGH);
    analogWrite(EnA, motorSpeedHigh);
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

// Outputs direction vehicle chose to travel and calculated distances
void printDirection(double distanceL, double distanceC, double distanceR)
{
  lcd.setCursor(0,0);
  lcd.print(String("Chose - ") + dir);
  lcd.setCursor(0,1);
  lcd.print(String(round(distanceL)) + "/" + String(round(distanceC)) + "/" + String(round(distanceR)) + "    ");
}

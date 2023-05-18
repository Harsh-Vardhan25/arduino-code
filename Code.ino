#include<Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial
char auth[] = "KxRpGCuOKZSon_OKW_SJKqRQfyUodNLh";
char ssid[] = "Harsh's Galaxy F14 5G";
char pass[] = "qaereh25bbsa4s2";
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
#define ROW_NUM 4 // four rows
#define COLUMN_NUM 3 // three columns
char keys[ROW_NUM][COLUMN_NUM] = {
 {'1', '2', '3'},
 {'4', '5', '6'},
 {'7', '8', '9'},
 {'*', '0', '#'}
};
byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; // GIOP32, GIOP33, GIOP25, GIOP26 
connect to the row pins
byte pin_column[COLUMN_NUM] = {27, 14, 12}; // GIOP27, GIOP14, GIOP12 connect 
to the column pins
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, 
COLUMN_NUM );
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
int no_of_beakers; //2, 3, 4
int dip_duration;
int drip_duration;
int no_of_cycles;
int RPM;
int rpm;
//Stepper Motor
const int dirPin = 2;
const int stepPin = 15;
int stepsPerRevolution;
int stepsPerRevolution_2;
int stepDelay;
//Motor
const int MotorPin1 = 18;
const int MotorPin2 = 5;
const int MotorPin3 = 17;
const int MotorPin4 = 16;
int getKeypadInput(String label) {
 String input = "";
 char key = NO_KEY;
 lcd.setCursor(0, 0);
 lcd.print(label); // display the label on the first row of the LCD
 while (key != '#') {
  key = keypad.getKey(); // read the next key from the keypad
  if (key == '*') {
  ESP.restart();
  }
 if (key != NO_KEY) { // if a key was pressed
 input += key; // add the key to the input string
 lcd.setCursor(input.length(), 1);
 lcd.print(key); // display the key on the second row of the LCD
  }
 }
 int result = input.toInt(); // convert the input string to an integer
 lcd.clear();
 return result; // return the result
}
void setup() {
 Serial.begin(9600);
 if (WiFi.status() == WL_CONNECTED) {
  Blynk.begin(auth, WiFi, "blynk.cloud", 80);
 }
 pinMode(stepPin, OUTPUT);
 pinMode(dirPin, OUTPUT);
 pinMode(MotorPin1, OUTPUT);
 pinMode(MotorPin2, OUTPUT);
 pinMode(MotorPin3, OUTPUT);
 pinMode(MotorPin4, OUTPUT);
 pinMode(19, OUTPUT);
 pinMode(4, OUTPUT);
 digitalWrite(19, HIGH);
//500 rpm motor pulls the assembly upwards
 digitalWrite(MotorPin1, HIGH);
 digitalWrite(MotorPin2, LOW);
 delay(4000);
 digitalWrite(19, LOW);
 lcd.begin();
 lcd.backlight();
 no_of_beakers = getKeypadInput("Beakers:");
 dip_duration = getKeypadInput("Dip Duration:");
 drip_duration = getKeypadInput("Drip Duration:");
 no_of_cycles = getKeypadInput("Cycles:");
 RPM = getKeypadInput("Speed in RPM: ");
 stepsPerRevolution = 200 / no_of_beakers;
 stepsPerRevolution_2 = (no_of_beakers - 1) * stepsPerRevolution;
 stepDelay = 10;
 rpm = RPM * (255 / 200);
 lcd.setCursor(0, 0);
 lcd.print("Inputs given: ");
 delay(1000);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Beakers: ");
 lcd.print(no_of_beakers);
 lcd.setCursor(0, 1);
 lcd.print("Dip Duration: ");
 lcd.print(dip_duration);
 delay(2000);
 lcd.clear();
 delay(800);
 lcd.setCursor(0, 0);
 lcd.print("Drip Duration: ");
 lcd.print(drip_duration);
 lcd.setCursor(0, 1);
 lcd.print("Cycles: ");
 lcd.print(no_of_cycles);
 delay(2000);
 lcd.clear();
 delay(800);
 lcd.setCursor(0, 0);
 lcd.print("Speed in RPM: ");
 lcd.setCursor(0, 1);
 lcd.print(rpm);
 delay(2000);
 lcd.clear();
 Blynk.virtualWrite(V1, no_of_beakers);
 Blynk.virtualWrite(V2, dip_duration);
 Blynk.virtualWrite(V3, drip_duration);
 Blynk.virtualWrite(V4, rpm);
}
void loop() {
 digitalWrite(19, HIGH);
 Blynk.virtualWrite(V5, no_of_cycles);
 lcd.clear();
 delay(500);
 lcd.setCursor(0, 0);
 lcd.print("Cycles left: ");
 lcd.setCursor(0, 1);
 lcd.print(no_of_cycles);
// main code here, to run repeatedly:
 if (no_of_cycles--) {
 //Turn 500 rpm Motor for 1962.5 millisecond
 for (int i = 0; i < no_of_beakers - 1; i++) {
 digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, HIGH);
 digitalWrite(MotorPin3, LOW);
 digitalWrite(MotorPin4, LOW);
 delay(1962.5);
 //Turn 200 rpm motor for Dip Duration
 digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, HIGH);
 digitalWrite(MotorPin4, LOW);
 analogWrite(4, rpm);
 delay(dip_duration * 1000);
 //Turn 500 rpm Motor in opposite direction for 4500 ms
 digitalWrite(MotorPin1, HIGH);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, LOW);
 digitalWrite(MotorPin4, LOW);
 delay(4500);
 //Drip Duration
 digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, LOW);
 digitalWrite(MotorPin4, LOW);
 delay(drip_duration * 1000);
 //Rotate Stepper motor clockwise
 anticlockwise();
 }
 digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, HIGH);
 digitalWrite(MotorPin3, LOW);
 digitalWrite(MotorPin4, LOW);
 delay(1962.5);
 //Turn 200 rpm motor in clockwise for Dip Duration
digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, HIGH);
 digitalWrite(MotorPin4, LOW);
 analogWrite(4, rpm);
 delay(dip_duration * 1000);
 //Turn 500 rpm Motor in opposite direction for 4500 ms
 digitalWrite(MotorPin1, HIGH);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, LOW);
 digitalWrite(MotorPin4, LOW);
 delay(4500);
 //Drip Duration
 digitalWrite(MotorPin1, LOW);
 digitalWrite(MotorPin2, LOW);
 digitalWrite(MotorPin3, LOW);
digitalWrite(MotorPin4, LOW);
 delay(drip_duration * 1000);
 clockwise();
}
else {
 lcd.clear();
 delay(500);
 lcd.setCursor(0, 0);
 lcd.print("Cycles");
 lcd.setCursor(0, 1);
 lcd.print("Completed");
 exit(0);
 }
}
void clockwise() {
 digitalWrite(dirPin, HIGH);
// Spin motor
 for (int x = 0; x < stepsPerRevolution_2; x++)
 {
 digitalWrite(stepPin, HIGH);
 delay(stepDelay);
 digitalWrite(stepPin, LOW);
 delay(stepDelay);
 }
 delay(1000); // Wait a second
}
void anticlockwise()
{
 //counterclockwise
 digitalWrite(dirPin, LOW);
 // Spin motor
 for (int x = 0; x < stepsPerRevolution; x++)
 {
 digitalWrite(stepPin, HIGH);
 delay(stepDelay);
 digitalWrite(stepPin, LOW);
 delay(stepDelay);
 }
 delay(1000); // Wait a second
}

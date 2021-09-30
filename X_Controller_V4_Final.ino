// All "Serial.print()" statements are for debugging using the serial monitor if rover is not being responsive
//========================Library======================//
#include <SoftwareSerial.h>
#include <ArduinoBlue.h>
#include <HCSR04.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include<UltraDistSensor.h>
//=====================================================//
#define DHTPIN 2
//Defines which type of sensor I will be using
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//========================Int Setup=====================//
Servo motor; // creates a servo object to control Drive Motor
Servo wheel; // creates a servo object to control Steering Motor
Servo Pan; // creates a servo object to control camera Pan 
Servo Tilt; // creates a servo object to control camera Tilt
UltraDistSensor DST; //initialisation class HCSR04 (trig pin , echo pin)
// Bluetooth TX -> Arduino D2
const int BLUETOOTH_TX = 51;
// Bluetooth RX -> Arduino D3
const int BLUETOOTH_RX = 50;
// 
int throttle, steering, sliderVal, button, sliderID;
float distance;
int wheel_left, wheel_right, wheel_center;// Steering Servo variables
int forwards_fast, forwards_medium, forwards_slow, backwards_fast, backwards_medium, backwards_slow, throttle_off; // Motor Servo Varables

//=====================================================//

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);
ArduinoBlue phone(bluetooth); // pass reference of bluetooth object to ArduinoBlue constructor.
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Setup code runs once after program starts.
void setup() {
  
    // Servo/Motor Setup
    motor.attach(30); // attaches the servo on pin 30 to the servo motor object
    wheel.attach(40); // attaches the servo on pin 24 to the servo wheel object
    Pan.attach(24); // attaches the servo on pin 24 to the servo pan object
    Tilt.attach(26); // attaches the servo on pin 26 to the servo tilt object
    motor.write(90); // Activating Motor
    motor.write(91); // Activating Motor
    
    // Hummidity and Temperature Senseor Setup
    dht.begin();

    // Ultra Sonic Sound Sensor
    DST.attach(38,39); // Trigger pin, Echo pin

    // LED Lights SetUp
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
    
    // Start serial monitor at 9600 bps.
    Serial.begin(9600);

    // Start bluetooth serial at 9600 bps.
    bluetooth.begin(9600);
    // delay just in case bluetooth module needs time to "get ready".
    delay(100);

    //==============Motor Activation Sequence==============//
    motor.write(90);delay(50);
    motor.write(91);delay(50);
    motor.write(90);delay(50);
    motor.write(91);delay(50);
    motor.write(90);delay(50);
    motor.write(91);delay(50);
    motor.write(90);delay(50);
    motor.write(91);delay(50);
    //=====================================================//

    // default//
    wheel_left = 140;
    wheel_right = 50;
    wheel_center = 88;
    forwards_fast = 300;
    forwards_medium = 105;
    forwards_slow = 98;
    backwards_fast = 30;
    backwards_medium = 50;
    backwards_slow = 70;
    throttle_off = 90;

    lcd.init();// initialize the lcd 
    lcd.init();// initialize the lcd
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("setup complete");
}

// Put your main code here, to run repeatedly:
void loop() {
    //========================Motor Arming======================//
    // Just incase motor still hasn't activated
    motor.write(91);delay(50); //motor arming
    motor.write(90);delay(50); //motor arming
    //==========================================================//

    //========================DHT22 SetUp======================//
    
    //========================================================//
    
    
    //========================Data Setup======================//
    // ID of the button pressed pressed.
    button = phone.getButton();
    
    // ID of the slider moved
    sliderID = phone.getSliderId();
    //Slider value. From 0 to 200 defualt being 90)
    sliderVal = phone.getSliderVal();
  
    // Returns the text data sent from the phone.
    // After it returns the latest data, empty string "" is sent in subsequent.
    // calls until text data is sent again.
    String str = phone.getText();
    //==========================================================//
    
    //================================Main==================================//
    //===========================Button Logic===============================//
    // Display button data whenever its pressed if connected.
    // Convert button ID to command for arduino to follow  
    if (button != -1) 
    {
        Serial.print("Button: ");
        Serial.println(button);
        if (button == 8)// Accelerate Controlls 
        {
          Serial.print("Forward 1 second:");
          motor.write(forwards_slow); delay(1000);
        }
        else if (button == 9)// Accelerate Controlls 
        {
          Serial.print("Forward 2 seconds:");
          motor.write(forwards_slow); delay(2000);
        }
        else if (button == 10)// Accelerate Controlls 
        {
          Serial.print("Forward 5 seconds:");;
          motor.write(forwards_slow); delay(5000);
        }
        else if (button == 11)// Reverse Controlls
        {
          Serial.print("Backwards 1 second:");
          motor.write(backwards_slow); delay(1000);
        }
        else if (button == 12)// Reverse Controlls
        {
          Serial.print("Backwards 2 seconds:");
          motor.write(backwards_slow); delay(2000);
        }
        else if (button == 13)// Reverse Controlls 
        {
          Serial.print("Backwards 5 seconds:");
          motor.write(backwards_slow); delay(5000);
        }
        else if (button == 14)// Light On
        {
          Serial.print("Lights On:");
          digitalWrite(8, HIGH);
          digitalWrite(9, HIGH);          
        }
        else if (button == 15)// Light Off
        {
          Serial.print("Lights Off:");
          digitalWrite(8, LOW);
          digitalWrite(9, LOW);
        }
        else if (button == 16)// Display Distance 
        {
          Serial.print("Display Distance");
          DisplayDistance();
        }
        else if (button == 17)// Display Temperature and Humidity Data
        {
          Serial.print("Temp and Hum Data");
          DisplayTempHum();    
        }
        else if (button == 18)// Clear the Display
        {
          Serial.print("Clear Display");
          ClearScreen();
        }
        else if (button == 19)// Test Dispaly 
        {
          Serial.print("Test Display");
          TestScreen();
        }else{
          Serial.print("Unkown Command");
          Serial.print("//==========================================================//");
        }
    }
    //===========================Slider Logic===============================//
    // Display slider data whenever its changed if connected.
    // Convert Slider ID to command for arduino to follow  
        if (sliderID != -1) 
    {
        Serial.print("Silder: ");
        Serial.println(sliderID);
        if (sliderID == 5)// Camera Pan Control
        {
          Serial.print("Camera Pan Value:");
          Serial.print(sliderVal);
          Pan.write(sliderVal);
        }
        else if (sliderID == 6)// Camera Tilt Control
        {
          Serial.print("Camera Tilt Value:");
          Serial.print(sliderVal);
          Tilt.write(sliderVal);
        }
        else if (sliderID == 7)// Steering Control
        {
          Serial.print("Steering Value:");
          Serial.print(sliderVal);
          wheel.write(sliderVal);
        }
        else
        {
          Serial.print("Unkown Command");
          Serial.print("//==========================================================//");
        }
    }

    // If a text from the phone was sent print it to the serial monitor
    if (str != "") 
    {
        Serial.println(str);
    }

    // Send string from serial command line to the phone. This will alert the user.
    if (Serial.available()) {
        Serial.write("send: ");
        String str = Serial.readString();
        phone.sendMessage(str); // phone.sendMessage(str) sends the text to the phone.
        Serial.print(str);
        Serial.write('\n');
    }
}


void DisplayDistance()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Processing..");
  delay(750);
  lcd.clear();
  lcd.setCursor(0,0);
  distance=DST.distanceInCm();
  lcd.print("Distance: ~");
  lcd.print(distance);
  return;
}

void DisplayTempHum()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Processing..");
  
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperature:");
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print("%");
  
  return;
}

void ClearScreen()
{
  lcd.clear();
  return ;  
}

void TestScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Testing 12345678");
  lcd.setCursor(0,1);
  lcd.print("Testing 87654321");
  delay(1500);
  lcd.clear();
  return;
}

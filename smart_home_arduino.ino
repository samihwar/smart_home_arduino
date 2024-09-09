#include "spiUnit.h"
#include "TFT9341.h"
#include "touch.h"

#define FAN_PIN A3
#define MOTION_PIN A4
#define TEMP_SENSOR_PIN A7
#define LDR_PIN A6
#define RED_PIN 3
#define GREEN_PIN 4
#define BLUE_PIN 5

enum FanState { OFF, ON, AUTOMATIC };
FanState fanState = AUTOMATIC; // Fan starts in the AUTOMATIC state

enum LEDState { LED_OFF, LED_ON };
LEDState ledState = LED_ON; // LED starts in ON mode

int tempOverCount = 0; // Count of consecutive temperature readings over 30°C
int x, y;
bool motionDetected = false;
bool lastMotionState = false;

void setup() {
    Serial.begin(9600);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(MOTION_PIN, INPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    
    lcd.begin();
    lcdtouch.begin();
    lcd.setRotation(2);
    lcd.clrscr();
    lcd.setFont(3);
    
    // Draw On/Off buttons for LED at the top
    lcd.fillRoundRect(10, 50, 80, 50, 5, YELLOW);
    lcd.setColor(WHITE, YELLOW);
    lcd.gotoxy(25, 65);
    lcd.print("LED ON");
    
    lcd.fillRoundRect(120, 50, 80, 50, 5, GRAY);
    lcd.setColor(WHITE, GRAY);
    lcd.gotoxy(135, 65);
    lcd.print("LED OFF");
    
    // Draw On/Off/Auto buttons for fan below the LED buttons
    lcd.fillRoundRect(10, 180, 80, 50, 5, GREEN);
    lcd.setColor(WHITE, GREEN);
    lcd.gotoxy(35, 195);
    lcd.print("ON");
    
    lcd.fillRoundRect(120, 180, 80, 50, 5, RED);
    lcd.setColor(WHITE, RED);
    lcd.gotoxy(135, 195);
    lcd.print("OFF");
    
    lcd.fillRoundRect(230, 180, 80, 50, 5, BLUE);
    lcd.setColor(WHITE, BLUE);
    lcd.gotoxy(235, 195);
    lcd.print("AUTO");
}

void loop() {
    // Check for touch input
    lcdtouch.readxy();
    x = lcdtouch.readx();
    y = lcdtouch.ready();
    
    // Print the coordinates to the Serial Monitor
    Serial.print("Touched at X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.println(y);
    
    // Check if ON button for the fan is pressed
    if (x > 10 && x < 90 && y > 180 && y < 230) {
        fanState = ON;
        digitalWrite(FAN_PIN, HIGH);
        Serial.println("Fan ON (Manual)");
    }
    
    // Check if OFF button for the fan is pressed
    if (x > 120 && x < 200 && y > 180 && y < 230) {
        fanState = OFF;
        digitalWrite(FAN_PIN, LOW);
        Serial.println("Fan OFF (Manual)");
    }
    
    // Check if AUTO button for the fan is pressed
    if (x > 230 && x < 310 && y > 180 && y < 230) {
        fanState = AUTOMATIC;
        Serial.println("Fan in AUTOMATIC mode");
    }

    // Check if LED ON button is pressed
    if (x > 10 && x < 90 && y > 50 && y < 100) {
        ledState = LED_ON;
        Serial.println("LED ON Mode");
    }
    
    // Check if LED OFF button is pressed
    if (x > 120 && x < 200 && y > 50 && y < 100) {
        ledState = LED_OFF;
        Serial.println("LED OFF Mode");
    }

    // Handle fan control in AUTOMATIC mode
    int tempValue = analogRead(TEMP_SENSOR_PIN);
    float voltage = tempValue * (5.0 / 1023.0); // Convert ADC value to voltage
    float tempC = voltage * 100; // LM35: 10mV per degree Celsius

    if (fanState == AUTOMATIC) {
        if (tempC > 30) {
            tempOverCount++;
        } else {
            tempOverCount = 0;
        }

        if (tempOverCount >= 3) {
            digitalWrite(FAN_PIN, HIGH);
            Serial.println("Fan ON (Automatic: 3+ consecutive readings > 30°C)");
        } else if (tempOverCount < 3) {
            digitalWrite(FAN_PIN, LOW);
            Serial.println("Fan OFF (Automatic: Temperature <= 30°C)");
        }
    }

    // Handle RGB LED behavior based on LED state
    int ldrValue = analogRead(LDR_PIN);
    motionDetected = digitalRead(MOTION_PIN);

    if (motionDetected != lastMotionState) {
        if (motionDetected) {
            Serial.println("Motion Detected!");
        } else {
            Serial.println("No Motion Detected");
        }
        lastMotionState = motionDetected;
    }

    if (ledState == LED_ON) {
        if (motionDetected) {
            // Police light effect: alternate between blue and red
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, HIGH);
            delay(300);
            digitalWrite(BLUE_PIN, LOW);
            digitalWrite(RED_PIN, HIGH);
            delay(300);
        } else {
            // Green light on when no motion is detected
            digitalWrite(RED_PIN, LOW);
            digitalWrite(BLUE_PIN, LOW);
            digitalWrite(GREEN_PIN, HIGH);
        }
    } else if (ledState == LED_OFF) {
        if (ldrValue < 200) {
            // White light when LDR is under 200
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, HIGH);
        } else {
            // Turn off the LED
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, LOW);
        }
    }
}

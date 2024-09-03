#include "spiUnit.h"
#include "TFT9341.h"
#include "touch.h"

#define FAN_PIN A3
#define MOTION_PIN A4
#define TEMP_SENSOR_PIN A7
#define LDR_PIN A6

int fanState = 0; // Fan off initially
int x, y;

void setup() {
    Serial.begin(9600);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(MOTION_PIN, INPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    
    lcd.begin();
    lcdtouch.begin();
    lcd.setRotation(2);
    lcd.clrscr();
    lcd.setFont(3);
    
    // Draw On/Off buttons
    lcd.fillRoundRect(50, 180, 100, 50, 5, YELLOW);
    lcd.setColor(BLUE, YELLOW);
    lcd.gotoxy(80, 195);
    lcd.print("ON");
    
    lcd.fillRoundRect(200, 180, 100, 50, 5, YELLOW);
    lcd.setColor(BLUE, YELLOW);
    lcd.gotoxy(225, 195);
    lcd.print("OFF");
    
    // Initialize the display for sensor readings
    updateSensorDisplay();
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
    
    // Check if ON button is pressed
    if (x > 50 && x < 200 && y > 180 && y < 230) {
        fanState = 1;
        digitalWrite(FAN_PIN, HIGH);
        Serial.println("Fan ON");
    }
    
    // Check if OFF button is pressed
    if (x > 200 && x < 300 && y > 180 && y < 230) {
        fanState = 0;
        digitalWrite(FAN_PIN, LOW);
        Serial.println("Fan OFF");
    }
    
    // Update sensor readings
    updateSensorDisplay();
    
    delay(500); // Small delay for readability
}

void updateSensorDisplay() {
    int motionDetected = digitalRead(MOTION_PIN);
    int tempValue = analogRead(TEMP_SENSOR_PIN);
    int ldrValue = analogRead(LDR_PIN);
    
    // Clear the area before updating values
    lcd.fillRect(10, 10, 300, 150, BLACK);
    
    lcd.setColor(WHITE, BLACK);
    lcd.gotoxy(10, 30);
    lcd.print("Motion: ");
    lcd.print(motionDetected ? "Detected" : "None");
    
    lcd.gotoxy(10, 60);
    lcd.print("Temp: ");
    lcd.print(tempValue);
    
    lcd.gotoxy(10, 90);
    lcd.print("LDR: ");
    lcd.print(ldrValue);
}

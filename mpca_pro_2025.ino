#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int pirPin = 8;         // PIR sensor output pin
const int buzzerPin = 3;      // Buzzer signal (I/O) pin
const int tempPin = A0;       // LM35 temperature sensor analog pin
const int uvPin = A1;         // UV sensor analog pin

int pirState = LOW;
int val = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  Serial.println("PIR + Buzzer + Temp System Initialized...");
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(30000);  // PIR sensor warm-up

  Serial.println("System Ready.");
  lcd.clear();
}

void loop() {
  val = digitalRead(pirPin);

  if (val == HIGH) {
    if (pirState == LOW) {
      unsigned long timestamp = millis() / 1000;

      // Read LM35 temperature
    int sensorValue = analogRead(tempPin);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperatureC = (voltage * 100.0) - 15.0; // crude fix



      // Serial output with timestamp
      Serial.print("🚨 Motion Detected at ");
      Serial.print(timestamp);
      Serial.print(" sec | LM35 Temp: ");
      Serial.print(temperatureC);
      Serial.println(" °C");

      // LCD output
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Motion at ");
      lcd.print(timestamp);
      lcd.print("s");

      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(temperatureC, 1);
      lcd.print((char)223);  // degree symbol
      lcd.print("C");

      // Buzzer
      tone(buzzerPin, 1000); delay(300);
      noTone(buzzerPin);     delay(100);
      tone(buzzerPin, 1200); delay(300);
      noTone(buzzerPin);

      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      unsigned long timestamp = millis() / 1000;
      Serial.print("✅ Motion Ended at ");
      Serial.print(timestamp);
      Serial.println(" sec");

      lcd.clear();
      pirState = LOW;
    }

    // UV-based temperature estimation
    int uvReading = analogRead(uvPin);
    float uvVoltage = uvReading * (5.0 / 1023.0);
    float uvIntensity = uvVoltage * 307.0;  // µW/cm²
    float ambientTemp = (0.028 * uvIntensity + 20.0) * 0.95;
 // Approx model

    unsigned long timestamp = millis() / 1000;

    Serial.print("🌤 Ambient at ");
    Serial.print(timestamp);
    Serial.print(" sec | UV Temp: ");
    Serial.print(ambientTemp, 1);
    Serial.println(" °C");

    lcd.setCursor(0, 0);
    lcd.print("No Object Found ");
    lcd.setCursor(0, 1);
    lcd.print("UV Temp: ");
    lcd.print(ambientTemp, 1);
    lcd.print((char)223);
    lcd.print("C");
  }

  delay(1000);  // Delay between checks
}

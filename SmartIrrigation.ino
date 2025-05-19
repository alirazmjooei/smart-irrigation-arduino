#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// آدرس I2C معمول LCD (اغلب 0x27 یا 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = A0;
const int relayPin = 10;

const int dryThreshold = 700;
const int wetThreshold = 500;

bool isWatering = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // رله خاموش (Active LOW)
}

void loop() {
  int moistureValue = analogRead(sensorPin);

  Serial.print("Moisture Level: ");
  Serial.println(moistureValue);

  if (!isWatering && moistureValue > dryThreshold) {
    digitalWrite(relayPin, LOW);  // روشن کردن رله (Active LOW)
    isWatering = true;
    Serial.println("Watering started...");
  } 
  else if (isWatering && moistureValue < wetThreshold) {
    digitalWrite(relayPin, HIGH);  // خاموش کردن رله
    isWatering = false;
    Serial.println("Watering stopped.");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moistureValue);

  lcd.setCursor(0, 1);
  if (isWatering) {
    lcd.print("Status: Watering");
  } else {
    lcd.print("Status: Idle     ");
  }

  delay(2000);
}

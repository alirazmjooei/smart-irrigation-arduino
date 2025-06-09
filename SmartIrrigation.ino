#include <SD.h>
#include <SPI.h>
#include <DHT.h>

// --- پایه‌ها ---
const int soilPin = A0;    // سنسور رطوبت خاک (YL-69) ← آنالوگ
const int dhtPin = A1;     // سنسور DHT11 ← دیجیتال
const int lightPin = A2;   // سنسور نور (LDR) ← آنالوگ
const int relayPin = 10;   // رله برای پمپ آب ← دیجیتال
const int chipSelect = 4;  // پایه CS کارت SD ← دیجیتال

// --- آستانه‌ها ---
const int dryThreshold = 700;
const int wetThreshold = 500;

bool isWatering = false;
File logFile;

// --- DHT ---
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

// --- زمان‌بندی ذخیره‌سازی ---
unsigned long lastLogTime = 0;
const unsigned long logInterval = 30000; // هر ۳۰ ثانیه

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // پمپ خاموش در ابتدا

  dht.begin();

  // راه‌اندازی SD کارت
  if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
  } else {
    Serial.println("SD init done.");
    logFile = SD.open("analog.csv", FILE_WRITE);
    if (logFile && logFile.size() == 0) {
      logFile.println("Time(ms),SoilAnalog,LightAnalog,Temp(C),Humidity(%),Watering");
      logFile.close();
    }
  }
}

void loop() {
  unsigned long currentTime = millis();

  // خواندن مقادیر سنسورها
  int soilValue = analogRead(soilPin);
  int lightValue = analogRead(lightPin);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // منطق آبیاری
  if (!isWatering && soilValue > dryThreshold) {
    digitalWrite(relayPin, LOW); // پمپ روشن
    isWatering = true;
    Serial.println("Watering started...");
  } else if (isWatering && soilValue < wetThreshold) {
    digitalWrite(relayPin, HIGH); // پمپ خاموش
    isWatering = false;
    Serial.println("Watering stopped.");
  }

  // چاپ روی سریال مانیتور
  Serial.print("Soil: "); Serial.print(soilValue);
  Serial.print(" | Light: "); Serial.print(lightValue);
  Serial.print(" | Temp: "); Serial.print(temperature);
  Serial.print("°C | Humidity: "); Serial.print(humidity);
  Serial.print("% | Watering: ");
  Serial.println(isWatering ? "Yes" : "No");

  // ذخیره اطلاعات در SD کارت هر ۳۰ ثانیه
  if (currentTime - lastLogTime >= logInterval) {
    logFile = SD.open("analog.csv", FILE_WRITE);
    if (logFile) {
      logFile.print(currentTime);
      logFile.print(",");
      logFile.print(soilValue);
      logFile.print(",");
      logFile.print(lightValue);
      logFile.print(",");
      logFile.print(temperature);
      logFile.print(",");
      logFile.print(humidity);
      logFile.print(",");
      logFile.println(isWatering ? "Yes" : "No");
      logFile.close();
      lastLogTime = currentTime;

      // پیام ذخیره موفق
      Serial.println("✔ Data logged to SD card.");
    } else {
      Serial.println("Error writing to SD!");
    }
  }

  delay(1000); // حلقه هر ۱ ثانیه
}

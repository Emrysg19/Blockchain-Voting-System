#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fingerprint sensor setup (AS608 on GPIO16=RX2, GPIO17=TX2)
HardwareSerial fpSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fpSerial);

// Buffer for incoming JSON from Node server
String inputBuffer = "";

// Forward declarations
void processCommand(JsonDocument &doc);
void enrollFingerprint(int id, const char *voterId);
void verifyFingerprint(const char *voterId);
void sendJson(JsonDocument &doc);
void clearFingerprintDatabase();


void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("AS608 Probe...");
  
  // Start fingerprint sensor on 57600 baud
  fpSerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  delay(1000);

  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("✅ Sensor Ready");
    Serial.println("✅ Found fingerprint sensor!");
  } else {
    lcd.clear();
    lcd.print("Sensor not found");
    Serial.println("❌ Fingerprint sensor not found. Check wiring.");
    while (1) delay(1);
  }
}

void loop() {
  // Handle serial input (JSON from Node server)
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      StaticJsonDocument<256> doc;
      DeserializationError err = deserializeJson(doc, inputBuffer);
      if (!err) {
        processCommand(doc);
      }
      inputBuffer = "";
    } else {
      inputBuffer += c;
    }
  }
}

void processCommand(JsonDocument &doc) {
  if (!doc.containsKey("action")) return;
  String action = doc["action"].as<String>();

  if (action == "ENROLL_BIOMETRIC") {
    const char *voterId = doc["voterId"];
    int fingerId = random(1, 200); // Assign ID dynamically (or from DB)
    enrollFingerprint(fingerId, voterId);
  }
  else if (action == "VERIFY_BIOMETRIC") {
  const char *voterId = doc["voterId"];
  verifyFingerprint(voterId);
}
else if (action == "CLEAR_BIOMETRIC_DB") {
    clearFingerprintDatabase();
}

}

void enrollFingerprint(int id, const char *voterId) {
  lcd.clear();
  lcd.print("Place finger...");
  Serial.println("Waiting for finger...");

  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p != FINGERPRINT_OK) {
      Serial.println("Error capturing image");
      return;
    }
  }

  // Convert image
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error image2Tz");
    return;
  }

  lcd.clear();
  lcd.print("Remove finger...");
  delay(2000);

  // Wait for finger to be removed
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  lcd.clear();
  lcd.print("Same finger...");
  Serial.println("Place same finger again");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p != FINGERPRINT_OK) {
      Serial.println("Error capturing 2nd image");
      return;
    }
  }

  // Convert 2nd image
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error image2Tz 2");
    return;
  }

  // Create model
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error createModel");
    return;
  }

  // Store model
  p = finger.storeModel(id);
  StaticJsonDocument<256> response;
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Enrolled OK!");
    response["type"] = "success";
    response["message"] = "Fingerprint enrolled";
    response["voterId"] = voterId;
    response["id"] = id;
    sendJson(response);
    
  } else {
    lcd.clear();
    lcd.print("Enroll failed");
    response["type"] = "error";
    response["error"] = "Enrollment failed";
    response["voterId"] = voterId;
    sendJson(response);
    
  }
}
void verifyFingerprint(const char *voterId) {
  lcd.clear();
  lcd.print("Place finger...");
  Serial.println("Waiting for finger...");

  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p != FINGERPRINT_OK) {
      Serial.println("Error capturing image");
      return;
    }
  }

  // Convert image
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error image2Tz");
    return;
  }

  // Search database
  p = finger.fingerSearch();
  StaticJsonDocument<256> response;

  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Verified!");
    response["type"] = "success";
    response["message"] = "Fingerprint verified";
    response["voterId"] = voterId;
    response["id"] = finger.fingerID;
    sendJson(response);
  } else {
    lcd.clear();
    lcd.print("No match");
    response["type"] = "error";
    response["error"] = "No match found";
    response["voterId"] = voterId;
    sendJson(response);
  }
}
void clearFingerprintDatabase() {
    lcd.clear();
    lcd.print("Clearing DB...");
    
    int p = finger.emptyDatabase();
    StaticJsonDocument<256> response;

    if (p == FINGERPRINT_OK) {
        response["type"] = "success";
        response["message"] = "All fingerprints cleared";
    } else {
        response["type"] = "error";
        response["error"] = "Failed to clear DB";
    }

    sendJson(response);

    lcd.clear();
    lcd.print("Ready for action");
}

void sendJson(JsonDocument &doc) {
  String output;
  serializeJson(doc, output);
  Serial.println(output);
}

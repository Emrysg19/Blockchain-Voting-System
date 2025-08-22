#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// Pin definitions
#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17
#define BUZZER_PIN 18

// WiFi credentials (update these with your network details)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Blockchain backend URL (update with your server URL)
const char* blockchainUrl = "http://your-blockchain-server.com/api/vote";

// System states
enum SystemState {
  STARTING,
  VOTING_MODE,
  PROCESSING
};

SystemState currentState = STARTING;

// Hardware objects
SoftwareSerial fingerprintSerial(FINGERPRINT_RX, FINGERPRINT_TX);
Adafruit_Fingerprint finger(&fingerprintSerial);
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 columns, 4 rows

// Function prototypes
void setupWiFi();
void setupHardware();
void displayMessage(const String& message, int row = 0);
void clearDisplay();
void beepSuccess();
void beepError();
void beepLong();
void processFingerprintVoting();
bool sendVoteToBlockchain(const String& voterId);
void showVotingMode();

void setup() {
  Serial.begin(115200);
  Serial.println("Blockchain Voting System Starting...");
  
  setupHardware();
  setupWiFi();
  
  // Initialize fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
    displayMessage("System Ready", 0);
    displayMessage("Starting voting mode...", 1);
    delay(2000);
    currentState = VOTING_MODE;
    showVotingMode();
  } else {
    Serial.println("Fingerprint sensor not found!");
    displayMessage("Sensor Error!", 0);
    displayMessage("Check connections", 1);
    while (1) {
      delay(1000); // Halt if sensor not found
    }
  }
}

void loop() {
  switch (currentState) {
    case VOTING_MODE:
      processFingerprintVoting();
      break;
    case PROCESSING:
      // Wait for processing to complete
      break;
    default:
      // STARTING state - do nothing
      break;
  }
  
  delay(100);
}

void setupHardware() {
  // Initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize fingerprint serial
  fingerprintSerial.begin(57600);
  
  Serial.println("Hardware initialized");
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void displayMessage(const String& message, int row) {
  lcd.setCursor(0, row);
  lcd.print(message);
}

void clearDisplay() {
  lcd.clear();
}

void beepSuccess() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepError() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepLong() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void showVotingMode() {
  clearDisplay();
  displayMessage("=== VOTING MODE ===", 0);
  displayMessage("Place finger to vote", 1);
  displayMessage("System is ready", 2);
  displayMessage("Waiting for voter...", 3);
  beepSuccess();
}

void processFingerprintVoting() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      return; // No finger detected
    }
    Serial.println("Error getting image");
    return;
  }
  
  // Convert image
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return;
  }
  
  // Search for fingerprint
  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOTFOUND) {
      displayMessage("Access Denied!", 2);
      displayMessage("Fingerprint not found", 3);
      beepLong();
      delay(2000);
      showVotingMode();
    } else {
      Serial.println("Error searching fingerprint");
    }
    return;
  }
  
  // Fingerprint found
  String voterId = "V" + String(finger.fingerID);
  displayMessage("Voter ID: " + voterId, 2);
  displayMessage("Processing vote...", 3);
  
  // Send vote to blockchain
  if (sendVoteToBlockchain(voterId)) {
    displayMessage("Vote Cast Successfully!", 2);
    displayMessage("Thank you for voting!", 3);
    beepSuccess();
  } else {
    displayMessage("Vote Failed!", 2);
    displayMessage("Please try again", 3);
    beepError();
  }
  
  delay(3000);
  showVotingMode();
}

bool sendVoteToBlockchain(const String& voterId) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }
  
  HTTPClient http;
  http.begin(blockchainUrl);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  StaticJsonDocument<200> doc;
  doc["voter_id"] = voterId;
  doc["timestamp"] = millis();
  doc["device_id"] = "ESP32_VOTING_001";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("Sending vote to blockchain: " + jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  bool success = false;
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      success = true;
    }
  } else {
    Serial.println("Error on HTTP request");
  }
  
  http.end();
  return success;
}
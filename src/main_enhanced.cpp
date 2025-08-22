#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include "BLOCKCHAIN_CONFIG.h"

// Pin definitions
#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17
#define BUZZER_PIN 18

// LCD dimensions
static const uint8_t LCD_COLS = 20;
static const uint8_t LCD_ROWS = 4;

// System states
enum SystemState {
  STARTING,
  VOTING_MODE,
  PROCESSING
};

SystemState currentState = STARTING;

// Hardware objects
HardwareSerial fingerprintSerial(2);
Adafruit_Fingerprint finger(&fingerprintSerial);
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Vote tracking
unsigned long lastVoteTime = 0;
int votesThisMinute = 0;
unsigned long lastMinuteReset = 0;

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
void resetVoteCounter();
bool isRateLimitExceeded();
String createVotePayload(const String& voterId);
void addAuthenticationHeaders(HTTPClient& http);
bool isSuccessResponse(int httpCode, const String& response);

void setup() {
  Serial.begin(115200);
  Serial.println("Enhanced Blockchain Voting System Starting...");
  
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
      delay(1000);
    }
  }
}

void loop() {
  // Reset vote counter every minute
  resetVoteCounter();
  
  switch (currentState) {
    case VOTING_MODE:
      processFingerprintVoting();
      break;
    case PROCESSING:
      break;
    default:
      break;
  }
  
  delay(100);
}

void setupHardware() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  fingerprintSerial.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
  
  Serial.println("Hardware initialized");
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi SSID: ");
  Serial.print(WIFI_SSID);
  
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect timed out. Check SSID/password and 2.4GHz availability.");
  }
}

void displayMessage(const String& message, int row) {
  if (row < 0) row = 0;
  if (row >= LCD_ROWS) row = LCD_ROWS - 1;
  // Clear the entire row to avoid leftover characters
  lcd.setCursor(0, row);
  for (uint8_t i = 0; i < LCD_COLS; i++) {
    lcd.print(' ');
  }
  // Write trimmed text (truncate if longer than display width)
  String text = message;
  if (text.length() > LCD_COLS) {
    text = text.substring(0, LCD_COLS);
  }
  lcd.setCursor(0, row);
  lcd.print(text);
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

void resetVoteCounter() {
  if (millis() - lastMinuteReset >= 60000) {
    votesThisMinute = 0;
    lastMinuteReset = millis();
  }
}

bool isRateLimitExceeded() {
  return votesThisMinute >= MAX_VOTES_PER_MINUTE;
}

void processFingerprintVoting() {
  // Check rate limiting
  if (isRateLimitExceeded()) {
    displayMessage("Rate limit exceeded", 2);
    displayMessage("Please wait...", 3);
    delay(2000);
    showVotingMode();
    return;
  }
  
  // Check cooldown
  if (millis() - lastVoteTime < VOTE_COOLDOWN_MS) {
    return;
  }
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      return;
    }
    Serial.println("Error getting image");
    return;
  }
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return;
  }
  
  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOTFOUND) {
      displayMessage("Access Denied!", 2);
      displayMessage("Print not found", 3);
      beepLong();
      delay(2000);
      showVotingMode();
    } else {
      Serial.println("Error searching fingerprint");
    }
    return;
  }
  
  String voterId = "V" + String(finger.fingerID);
  displayMessage("Voter ID: " + voterId, 2);
  displayMessage("Processing vote...", 3);
  
  if (sendVoteToBlockchain(voterId)) {
    displayMessage("Vote Cast Successfully!", 2);
    displayMessage("Thank you for voting!", 3);
    beepSuccess();
    votesThisMinute++;
    lastVoteTime = millis();
  } else {
    displayMessage("Vote Failed!", 2);
    displayMessage("Please try again", 3);
    beepError();
  }
  
  delay(3000);
  showVotingMode();
}

String createVotePayload(const String& voterId) {
  StaticJsonDocument<512> doc;
  
  // Required fields
  #if INCLUDE_VOTER_ID
    doc[FIELD_VOTER_ID] = voterId;
  #endif
  
  #if INCLUDE_TIMESTAMP
    doc[FIELD_TIMESTAMP] = millis();
  #endif
  
  #if INCLUDE_DEVICE_ID
    doc[FIELD_DEVICE_ID] = DEVICE_ID;
  #endif
  
  // Optional fields
  #if defined(INCLUDE_ELECTION_ID) && INCLUDE_ELECTION_ID
    doc[FIELD_ELECTION_ID] = DEFAULT_ELECTION_ID;
  #endif
  
  #if defined(INCLUDE_CANDIDATE_ID) && INCLUDE_CANDIDATE_ID
    doc[FIELD_CANDIDATE_ID] = DEFAULT_CANDIDATE_ID;
  #endif
  
  #if defined(INCLUDE_LOCATION) && INCLUDE_LOCATION
    doc[FIELD_LOCATION] = DEVICE_LOCATION;
  #endif
  
  #if defined(INCLUDE_POLLING_STATION) && INCLUDE_POLLING_STATION
    doc[FIELD_POLLING_STATION] = DEVICE_LOCATION;
  #endif
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  #if LOG_VOTE_DATA
    Serial.println("Vote payload: " + jsonString);
  #endif
  
  return jsonString;
}

void addAuthenticationHeaders(HTTPClient& http) {
  #if defined(USE_API_KEY) && USE_API_KEY
    http.addHeader(API_KEY_HEADER, API_KEY);
  #endif
  
  #if defined(USE_BEARER_TOKEN) && USE_BEARER_TOKEN
    http.addHeader(BEARER_HEADER, "Bearer " + String(BEARER_TOKEN));
  #endif
}

bool isSuccessResponse(int httpCode, const String& response) {
  // Check HTTP status code
  if (httpCode == 200 || httpCode == 201 || httpCode == 202) {
    return true;
  }
  
  // Parse response body if enabled
  #if defined(PARSE_RESPONSE_BODY) && PARSE_RESPONSE_BODY
    if (response.indexOf(SUCCESS_RESPONSE_KEY) != -1) {
      if (response.indexOf(SUCCESS_RESPONSE_VALUE) != -1) {
        return true;
      }
    }
  #endif
  
  return false;
}

bool sendVoteToBlockchain(const String& voterId) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }
  
  HTTPClient http;
  http.begin(BLOCKCHAIN_FULL_URL);
  http.addHeader("Content-Type", CONTENT_TYPE);
  
  // Add authentication headers if configured
  addAuthenticationHeaders(http);
  
  // Create vote payload
  String jsonString = createVotePayload(voterId);
  
  Serial.println("Sending vote to: " + String(BLOCKCHAIN_FULL_URL));
  
  int httpResponseCode = http.POST(jsonString);
  bool success = false;
  String response = "";
  
  if (httpResponseCode > 0) {
    response = http.getString();
    
    #if LOG_HTTP_RESPONSES
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    #endif
    
    success = isSuccessResponse(httpResponseCode, response);
  } else {
    Serial.println("Error on HTTP request");
  }
  
  http.end();
  return success;
}

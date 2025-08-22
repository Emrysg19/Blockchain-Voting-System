#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================

// WiFi credentials - UPDATE THESE WITH YOUR NETWORK DETAILS
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Blockchain backend server configuration
#define BLOCKCHAIN_SERVER_URL "http://your-blockchain-server.com"
#define BLOCKCHAIN_API_ENDPOINT "/api/vote"
#define BLOCKCHAIN_FULL_URL BLOCKCHAIN_SERVER_URL BLOCKCHAIN_API_ENDPOINT

// Network timeout settings
#define WIFI_TIMEOUT_MS 10000
#define HTTP_TIMEOUT_MS 5000
#define HTTP_RETRY_COUNT 3

// ============================================================================
// HARDWARE PIN CONFIGURATION
// ============================================================================

// Fingerprint sensor pins (UART)
#define FINGERPRINT_RX_PIN 16
#define FINGERPRINT_TX_PIN 17
#define FINGERPRINT_BAUDRATE 57600

// LCD display pins (I²C)
#define LCD_SDA_PIN 21
#define LCD_SCL_PIN 22
#define LCD_I2C_ADDRESS 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4

// Audio pin
#define BUZZER_PIN 18

// ============================================================================
// SYSTEM BEHAVIOR CONFIGURATION
// ============================================================================

// System startup timing
#define SYSTEM_STARTUP_DELAY_MS 2000
#define SENSOR_INIT_TIMEOUT_MS 10000

// Audio feedback timing
#define BEEP_SUCCESS_DURATION_MS 100
#define BEEP_ERROR_DURATION_MS 200
#define BEEP_LONG_DURATION_MS 1000
#define BEEP_VOLUME HIGH  // HIGH or LOW

// Display timing
#define MESSAGE_DISPLAY_TIME_MS 2000
#define PROCESSING_DISPLAY_TIME_MS 3000
#define ERROR_DISPLAY_TIME_MS 2000
#define VOTING_RESULT_DISPLAY_TIME_MS 3000

// Fingerprint sensor settings
#define FINGERPRINT_TIMEOUT_MS 10000
#define FINGERPRINT_RETRY_COUNT 3
#define FINGERPRINT_QUALITY_THRESHOLD 50

// ============================================================================
// SECURITY CONFIGURATION
// ============================================================================

// Device identification
#define DEVICE_ID "ESP32_VOTING_001"
#define DEVICE_VERSION "1.0.0"

// API security
#define API_KEY "YOUR_API_KEY"  // Optional: Add if your backend requires authentication
#define API_HEADER_NAME "X-API-Key"

// Vote validation
#define MAX_VOTES_PER_SESSION 100
#define VOTE_COOLDOWN_MS 5000  // Minimum time between votes from same device

// ============================================================================
// DEBUG AND LOGGING CONFIGURATION
// ============================================================================

// Serial communication
#define SERIAL_BAUDRATE 115200
#define DEBUG_MODE true

// Logging levels
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

#define CURRENT_LOG_LEVEL LOG_LEVEL_INFO

// ============================================================================
// CUSTOMIZATION OPTIONS
// ============================================================================

// LCD display messages - Customize these for your language/application
#define MSG_SYSTEM_READY "System Ready"
#define MSG_STARTING_VOTING "Starting voting mode..."
#define MSG_PLACE_FINGER "Place finger to vote"
#define MSG_SYSTEM_READY_STATUS "System is ready"
#define MSG_WAITING_FOR_VOTER "Waiting for voter..."
#define MSG_FINGERPRINT_REGISTERED "Fingerprint Registered!"
#define MSG_ACCESS_DENIED "Access Denied!"
#define MSG_VOTE_SUCCESS "Vote Cast Successfully!"
#define MSG_VOTE_FAILED "Vote Failed!"
#define MSG_PROCESSING "Processing..."
#define MSG_ERROR "Error!"
#define MSG_SENSOR_ERROR "Sensor Error!"
#define MSG_CHECK_CONNECTIONS "Check connections"

// System modes
#define MODE_VOTING "VOTING MODE"

// ============================================================================
// ADVANCED CONFIGURATION
// ============================================================================

// Memory management
#define JSON_DOCUMENT_SIZE 200
#define MAX_MESSAGE_LENGTH 50
#define MAX_VOTER_ID_LENGTH 20

// Performance tuning
#define MAIN_LOOP_DELAY_MS 100
#define FINGERPRINT_SCAN_INTERVAL_MS 100
#define LCD_UPDATE_INTERVAL_MS 500

// Error handling
#define MAX_CONSECUTIVE_ERRORS 5
#define ERROR_RECOVERY_DELAY_MS 1000

// ============================================================================
// FEATURE FLAGS
// ============================================================================

// Enable/disable features
#define ENABLE_WIFI true
#define ENABLE_BLOCKCHAIN_INTEGRATION true
#define ENABLE_AUDIO_FEEDBACK true
#define ENABLE_LCD_DISPLAY true
#define ENABLE_FINGERPRINT_SENSOR true
#define ENABLE_AUTOMATIC_STARTUP true

// Advanced features
#define ENABLE_VOTE_HISTORY false
#define ENABLE_MULTIPLE_ELECTIONS false
#define ENABLE_REMOTE_CONFIGURATION false
#define ENABLE_OTA_UPDATES false

// ============================================================================
// COMPILATION CHECKS
// ============================================================================

#if ENABLE_WIFI && !defined(WIFI_SSID)
#error "WiFi SSID must be defined when WiFi is enabled"
#endif

#if ENABLE_WIFI && !defined(WIFI_PASSWORD)
#error "WiFi password must be defined when WiFi is enabled"
#endif

#if ENABLE_BLOCKCHAIN_INTEGRATION && !defined(BLOCKCHAIN_SERVER_URL)
#error "Blockchain server URL must be defined when blockchain integration is enabled"
#endif

#if LCD_COLUMNS <= 0 || LCD_ROWS <= 0
#error "LCD dimensions must be positive numbers"
#endif

#if FINGERPRINT_BAUDRATE <= 0
#error "Fingerprint baudrate must be positive"
#endif

#endif // CONFIG_H

#ifndef BLOCKCHAIN_CONFIG_H
#define BLOCKCHAIN_CONFIG_H

// ============================================================================
// BLOCKCHAIN BACKEND CONFIGURATION
// ============================================================================
// This file contains all the configuration needed to adapt the ESP32
// voting system to your specific blockchain backend.
// Update these values according to your partner's blockchain system.

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================

// WiFi credentials - UPDATE THESE WITH YOUR NETWORK DETAILS
#define WIFI_SSID "AI_Bird_Dectector"
#define WIFI_PASSWORD "Bird.Ai.2025"

// Blockchain backend server configuration
#define BLOCKCHAIN_SERVER_URL "http://your-blockchain-server.com"
#define BLOCKCHAIN_API_ENDPOINT "/api/vote"
#define BLOCKCHAIN_FULL_URL BLOCKCHAIN_SERVER_URL BLOCKCHAIN_API_ENDPOINT

// Network timeout settings
#define WIFI_TIMEOUT_MS 10000
#define HTTP_TIMEOUT_MS 5000
#define HTTP_RETRY_COUNT 3

// ============================================================================
// API CONFIGURATION
// ============================================================================

// HTTP Method (usually POST for voting)
#define HTTP_METHOD "POST"

// Content Type
#define CONTENT_TYPE "application/json"

// Authentication (uncomment and configure if needed)
// #define USE_API_KEY true
// #define API_KEY "YOUR_API_KEY"
// #define API_KEY_HEADER "X-API-Key"

// #define USE_BEARER_TOKEN true
// #define BEARER_TOKEN "YOUR_BEARER_TOKEN"
// #define BEARER_HEADER "Authorization"

// ============================================================================
// VOTE DATA STRUCTURE
// ============================================================================
// Customize these fields to match your blockchain backend requirements

// Required fields (these are sent with every vote)
#define INCLUDE_VOTER_ID true
#define INCLUDE_TIMESTAMP true
#define INCLUDE_DEVICE_ID true

// Optional fields (uncomment and configure if needed)
// #define INCLUDE_ELECTION_ID true
// #define INCLUDE_CANDIDATE_ID true
// #define INCLUDE_LOCATION true
// #define INCLUDE_POLLING_STATION true
// #define INCLUDE_VOTE_HASH true

// Field names (customize these to match your backend expectations)
#define FIELD_VOTER_ID "voter_id"
#define FIELD_TIMESTAMP "timestamp"
#define FIELD_DEVICE_ID "device_id"
#define FIELD_ELECTION_ID "election_id"
#define FIELD_CANDIDATE_ID "candidate_id"
#define FIELD_LOCATION "location"
#define FIELD_POLLING_STATION "polling_station"
#define FIELD_VOTE_HASH "vote_hash"

// ============================================================================
// RESPONSE HANDLING
// ============================================================================

// Success response codes (any of these will be considered successful)
#define SUCCESS_HTTP_CODES "200,201,202"

// Parse response body for success confirmation (optional)
// #define PARSE_RESPONSE_BODY true
// #define SUCCESS_RESPONSE_KEY "success"
// #define SUCCESS_RESPONSE_VALUE "true"

// ============================================================================
// DEVICE CONFIGURATION
// ============================================================================

// Device identification
#define DEVICE_ID "ESP32_VOTING_001"
#define DEVICE_VERSION "1.0.0"
#define DEVICE_LOCATION "POLLING_STATION_A"

// Election configuration (if applicable)
#define DEFAULT_ELECTION_ID "ELECTION_2024"
#define DEFAULT_CANDIDATE_ID "CANDIDATE_1"

// ============================================================================
// SECURITY CONFIGURATION
// ============================================================================

// Enable HTTPS (recommended for production)
// #define USE_HTTPS true

// Certificate verification (if using HTTPS)
// #define VERIFY_SSL_CERT true

// Rate limiting
#define MAX_VOTES_PER_MINUTE 60
#define VOTE_COOLDOWN_MS 1000

// ============================================================================
// DEBUG AND LOGGING
// ============================================================================

// Enable detailed logging
#define ENABLE_DEBUG_LOGGING true

// Log vote data (be careful with sensitive information)
#define LOG_VOTE_DATA true

// Log HTTP responses
#define LOG_HTTP_RESPONSES true

// ============================================================================
// ERROR HANDLING
// ============================================================================

// Retry failed votes
#define ENABLE_VOTE_RETRY true
#define MAX_RETRY_ATTEMPTS 3
#define RETRY_DELAY_MS 5000

// Offline vote storage (if enabled)
// #define ENABLE_OFFLINE_STORAGE true
// #define MAX_OFFLINE_VOTES 100

// ============================================================================
// ADVANCED FEATURES
// ============================================================================

// Real-time status updates
// #define ENABLE_STATUS_UPDATES true
// #define STATUS_UPDATE_INTERVAL_MS 30000

// Device health monitoring
// #define ENABLE_HEALTH_MONITORING true
// #define HEALTH_CHECK_INTERVAL_MS 60000

// ============================================================================
// COMPILATION CHECKS
// ============================================================================

#if !defined(WIFI_SSID) || !defined(WIFI_PASSWORD)
#error "WiFi credentials must be defined"
#endif

#if !defined(BLOCKCHAIN_SERVER_URL)
#error "Blockchain server URL must be defined"
#endif

#if defined(USE_API_KEY) && !defined(API_KEY)
#error "API key must be defined when USE_API_KEY is enabled"
#endif

#if defined(USE_BEARER_TOKEN) && !defined(BEARER_TOKEN)
#error "Bearer token must be defined when USE_BEARER_TOKEN is enabled"
#endif

#endif // BLOCKCHAIN_CONFIG_H

# Blockchain-Based Voting System for ESP32

A secure, fingerprint-authenticated voting system that integrates with blockchain technology for transparent and tamper-proof voting records. The system starts automatically in voting mode and operates continuously without user intervention.

## Hardware Components

- **ESP32 Development Board** - Main microcontroller
- **AS608 Fingerprint Sensor** - Voter authentication
- **20x4 I²C LCD Display** - System status and user interface
- **Buzzer** - Audio feedback for operations

## Pin Connections

| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| AS608 RX  | GPIO 16   | Fingerprint sensor receive |
| AS608 TX  | GPIO 17   | Fingerprint sensor transmit |
| Buzzer    | GPIO 18   | Audio feedback |
| LCD SDA   | GPIO 21   | I²C data line |
| LCD SCL   | GPIO 22   | I²C clock line |

## System Features

### 🔐 **Fingerprint Authentication**
- Secure voter verification using registered fingerprints
- Real-time fingerprint matching for voter identification
- Prevents duplicate voting through biometric identification

### 🗳️ **Voting Process**
- Automatic startup in voting mode
- Simple one-touch voting after fingerprint verification
- Automatic blockchain integration for vote recording
- Real-time status updates on LCD display

### 🔒 **Security Features**
- Secure HTTP communication with blockchain backend
- Tamper-proof voting records
- Automatic error recovery and system resilience

### 📱 **User Interface**
- 20x4 LCD display with clear status messages
- Audio feedback (success/error beeps)
- Automatic operation without manual controls

## Setup Instructions

### 1. Hardware Assembly
1. Connect the AS608 fingerprint sensor to GPIO 16 (RX) and GPIO 17 (TX)
2. Connect the buzzer to GPIO 18
3. Connect the LCD display via I²C (GPIO 21 and 22)

### 2. Software Configuration
1. Update WiFi credentials in `src/main.cpp`:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

2. Update blockchain backend URL:
   ```cpp
   const char* blockchainUrl = "http://your-blockchain-server.com/api/vote";
   ```

3. Verify LCD I²C address (default: 0x27)

### 3. Building and Uploading
1. Install PlatformIO IDE or CLI
2. Open the project folder
3. Build and upload to your ESP32:
   ```bash
   pio run --target upload
   ```

## System Operation

### **Automatic Startup**
1. **Power On**: ESP32 boots and initializes all components
2. **WiFi Connection**: Automatically connects to configured network
3. **Sensor Verification**: Checks fingerprint sensor functionality
4. **Voting Mode**: Enters voting mode automatically
5. **Ready State**: Displays "Waiting for voter" message

### **Voting Workflow**
1. **Finger Detection**: System continuously scans for fingerprints
2. **Authentication**: Matches detected fingerprint against database
3. **Vote Processing**: Sends vote data to blockchain backend
4. **Result Display**: Shows success/failure message with audio feedback
5. **Return to Ready**: Automatically returns to waiting state

## System Workflow

```
[Power On] → [WiFi Connect] → [Sensor Check] → [Voting Mode]
     ↓
[Finger Scan] → [Authentication] → [Vote Processing]
     ↓
[Blockchain API] → [Result Display] → [Return to Ready]
```

## Blockchain Integration

The system sends HTTP POST requests to your blockchain backend with the following JSON payload:

```json
{
  "voter_id": "V123",
  "timestamp": 1234567890,
  "device_id": "ESP32_VOTING_001"
}
```

### Expected Response
- **HTTP 200/201**: Vote successfully recorded
- **Other codes**: Vote failed, retry required

## Troubleshooting

### **Fingerprint Sensor Issues**
- Check UART connections (GPIO 16/17)
- Verify sensor power supply (3.3V)
- Check baud rate (57600)

### **LCD Display Issues**
- Verify I²C address (use I²C scanner if needed)
- Check SDA/SCL connections
- Ensure proper power supply

### **WiFi Connection Issues**
- Verify SSID and password
- Check WiFi signal strength
- Ensure network compatibility (2.4GHz)

### **Blockchain API Issues**
- Verify server URL and endpoint
- Check network connectivity
- Review server logs for errors

## Security Considerations

1. **Network Security**: Use HTTPS for blockchain communication
2. **Data Privacy**: Ensure voter anonymity in blockchain records
3. **Physical Security**: Secure device access in voting locations
4. **System Integrity**: Automatic error recovery and continuous operation

## Customization

### **Adding New Features**
- Modify `SystemState` enum for additional modes
- Add new pin definitions for additional hardware
- Extend JSON payload for additional vote data

### **Changing Hardware**
- Update pin definitions in the code
- Modify library includes for different components
- Adjust timing parameters for different sensors

## Dependencies

- `WiFi` - ESP32 WiFi functionality
- `HTTPClient` - HTTP communication
- `ArduinoJson` - JSON parsing and creation
- `SoftwareSerial` - UART communication with fingerprint sensor
- `Adafruit_Fingerprint` - Fingerprint sensor library
- `LiquidCrystal_I2C` - LCD display control

## Important Notes

- **No Manual Controls**: System operates automatically without buttons
- **Continuous Operation**: Runs indefinitely until power is disconnected
- **Automatic Recovery**: Handles errors and returns to operational state
- **Production Ready**: Designed for unattended operation in voting locations

## License

This project is open source and available under the MIT License.

## Support

For technical support or feature requests, please open an issue in the project repository.

---

**Note**: This system is designed for educational and demonstration purposes. For production use in actual elections, additional security measures and compliance with local election laws should be implemented.

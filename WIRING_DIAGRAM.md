# Hardware Wiring Diagram

## ESP32 Pinout and Connections

```
                    ESP32 Development Board
                ┌─────────────────────────────────┐
                │                                 │
                │  ┌─────────────────────────┐   │
                │  │                         │   │
                │  │     USB Port            │   │
                │  │                         │   │
                │  └─────────────────────────┘   │
                │                                 │
                │  ┌─────────────────────────┐   │
                │  │                         │   │
                │  │     Boot Button         │   │
                │  │                         │   │
                │  └─────────────────────────┘   │
                │                                 │
                │  ┌─────────────────────────┐   │
                │  │                         │   │
                │  │     EN Button           │   │
                │  │                         │   │
                │  └─────────────────────────┘   │
                │                                 │
                │  ┌─────────────────────────┐   │
                │  │                         │   │
                │  │     GPIO Pins           │   │
                │  │                         │   │
                │  └─────────────────────────┘   │
                │                                 │
                └─────────────────────────────────┘
```

## Detailed Pin Connections

### 1. AS608 Fingerprint Sensor
```
AS608 Fingerprint Sensor Module
┌─────────────────────────────────┐
│                                 │
│  ┌─────────────────────────┐   │
│  │                         │   │
│  │    Fingerprint Area     │   │
│  │                         │   │
│  └─────────────────────────┘   │
│                                 │
│  ┌─────────────────────────┐   │
│  │                         │   │
│  │      Status LED         │   │
│  │                         │   │
│  └─────────────────────────┘   │
│                                 │
│  VCC  → 3.3V                   │
│  GND  → GND                     │
│  TX   → GPIO 16 (RX)           │
│  RX   → GPIO 17 (TX)           │
│                                 │
└─────────────────────────────────┘
```

**Connection Details:**
- **VCC**: Connect to 3.3V power supply
- **GND**: Connect to ESP32 ground
- **TX**: Connect to ESP32 GPIO 16 (RX)
- **RX**: Connect to ESP32 GPIO 17 (TX)

### 2. 20x4 I²C LCD Display
```
20x4 I²C LCD Display
┌─────────────────────────────────┐
│                                 │
│  ┌─────────────────────────┐   │
│  │                         │   │
│  │     20x4 Display       │   │
│  │                         │   │
│  └─────────────────────────┘   │
│                                 │
│  ┌─────────────────────────┐   │
│  │                         │   │
│  │     I²C Interface      │   │
│  │                         │   │
│  └─────────────────────────┘   │
│                                 │
│  VCC  → 3.3V                   │
│  GND  → GND                     │
│  SDA  → GPIO 21                │
│  SCL  → GPIO 22                │
│                                 │
└─────────────────────────────────┘
```

**Connection Details:**
- **VCC**: Connect to 3.3V power supply
- **GND**: Connect to ESP32 ground
- **SDA**: Connect to ESP32 GPIO 21 (I²C Data)
- **SCL**: Connect to ESP32 GPIO 22 (I²C Clock)

### 3. Buzzer
```
Piezo Buzzer
┌─────────────────────────────────┐
│                                 │
│  ┌─────────────────────────┐   │
│  │                         │   │
│  │      Piezo Element      │   │
│  │                         │   │
│  └─────────────────────────┘   │
│                                 │
│  Positive (+) → GPIO 18         │
│  Negative (-) → GND             │
│                                 │
└─────────────────────────────────┘
```

**Connection Details:**
- **Positive (+)** terminal: Connect to ESP32 GPIO 18
- **Negative (-)** terminal: Connect to ESP32 ground

## Complete Wiring Schematic

```
Power Supply (3.3V)
        │
        ├─── AS608 VCC
        ├─── LCD VCC
        └─── ESP32 3.3V
                │
                ├─── GPIO 16 ←── AS608 TX
                ├─── GPIO 17 ──→ AS608 RX
                ├─── GPIO 18 ──→ Buzzer (+)
                ├─── GPIO 21 ←── LCD SDA
                ├─── GPIO 22 ←── LCD SCL
                └─── GND ──────→ AS608 GND
                                ├─── LCD GND
                                └─── Buzzer (-)
```

## Power Requirements

### **Voltage Levels**
- **ESP32**: 3.3V (can accept 5V but operates at 3.3V)
- **AS608**: 3.3V
- **LCD**: 3.3V
- **Buzzer**: 3.3V (tolerant of 5V)

### **Current Requirements**
- **ESP32**: ~200mA (peak)
- **AS608**: ~50mA
- **LCD**: ~20mA
- **Buzzer**: ~30mA (when active)
- **Total**: ~300mA peak

### **Power Supply Recommendations**
- Use a stable 3.3V power supply rated for at least 500mA
- USB power from computer is sufficient for development
- For standalone operation, use a 3.3V regulator with adequate current capacity

## System Operation

### **Automatic Startup**
1. **Power On**: ESP32 boots automatically
2. **WiFi Connection**: Connects to configured network
3. **Sensor Initialization**: Verifies fingerprint sensor
4. **Voting Mode**: Automatically enters voting mode
5. **Ready State**: Displays "Waiting for voter" message

### **Voting Process**
1. **Finger Detection**: Continuously scans for fingerprints
2. **Authentication**: Matches fingerprint against database
3. **Vote Processing**: Sends vote to blockchain backend
4. **Result Display**: Shows success/failure message
5. **Return to Ready**: Automatically returns to waiting state

## Connection Tips

### **1. Wire Management**
- Use different colored wires for different signals
- Keep power and ground wires short and thick
- Route signal wires away from power wires to reduce interference

### **2. Soldering**
- Use proper soldering techniques for reliable connections
- Ensure good contact between wire and pin
- Use heat shrink tubing to insulate connections

### **3. Testing**
- Test each component individually before final assembly
- Use a multimeter to verify connections
- Check for short circuits before powering on

### **4. Troubleshooting**
- If LCD doesn't work, check I²C address with scanner
- If fingerprint sensor fails, verify UART connections
- If buzzer doesn't sound, check polarity and connections

## Safety Considerations

1. **Power Off**: Always disconnect power before making connections
2. **Voltage Check**: Verify all components receive correct voltage
3. **Current Limit**: Ensure power supply can handle total current
4. **Heat Management**: Some components may get warm during operation
5. **ESD Protection**: Handle sensitive components with care

## Notes

- **No Button Required**: System starts automatically in voting mode
- **Continuous Operation**: Runs indefinitely until power is disconnected
- **Automatic Recovery**: Returns to ready state after each vote
- **Error Handling**: Displays error messages and continues operation

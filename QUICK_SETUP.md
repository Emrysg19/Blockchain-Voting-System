# Quick Setup Guide - ESP32 to Your Blockchain

## 🚀 **Get Running in 5 Minutes**

### **Step 1: Update Configuration**
Edit `BLOCKCHAIN_CONFIG.h`:

```cpp
// Your WiFi
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Your blockchain server
#define BLOCKCHAIN_SERVER_URL "http://your-server.com"
#define BLOCKCHAIN_API_ENDPOINT "/api/vote"
```

### **Step 2: Your Backend Endpoint**
Create this endpoint on your server:

```javascript
// Example: Node.js/Express
app.post('/api/vote', (req, res) => {
  const { voter_id, timestamp, device_id } = req.body;
  
  // Store in your blockchain
  // ... your blockchain logic here ...
  
  res.status(200).json({ success: true });
});
```

### **Step 3: Upload & Test**
```bash
pio run --target upload
```

## 📋 **What You Need to Implement**

### **Required: HTTP POST Endpoint**
- **URL**: `/api/vote` (or your choice)
- **Method**: POST
- **Content-Type**: `application/json`
- **Response**: HTTP 200 for success, anything else for failure

### **Expected Data Format**
```json
{
  "voter_id": "V123",
  "timestamp": 1234567890,
  "device_id": "ESP32_VOTING_001"
}
```

### **Response Format**
- **Success**: HTTP 200, 201, or 202
- **Failure**: Any other HTTP code

## 🔧 **Advanced Configuration (Optional)**

### **Add Authentication**
```cpp
// In BLOCKCHAIN_CONFIG.h
#define USE_API_KEY true
#define API_KEY "your-api-key"
```

### **Custom Vote Fields**
```cpp
// Enable additional fields
#define INCLUDE_ELECTION_ID true
#define INCLUDE_CANDIDATE_ID true
#define INCLUDE_LOCATION true

// Customize field names
#define FIELD_ELECTION_ID "election_id"
#define FIELD_CANDIDATE_ID "candidate_id"
```

### **HTTPS Support**
```cpp
#define USE_HTTPS true
#define VERIFY_SSL_CERT true
```

## 🧪 **Test Your Integration**

### **1. Test with curl**
```bash
curl -X POST http://your-server.com/api/vote \
  -H "Content-Type: application/json" \
  -d '{"voter_id":"TEST123","timestamp":1234567890,"device_id":"ESP32_TEST"}'
```

### **2. Monitor ESP32 Serial Output**
Connect to ESP32 via USB and watch the serial monitor for:
- WiFi connection status
- Vote requests being sent
- Response codes and data

### **3. Check Your Backend Logs**
Verify that votes are reaching your blockchain system.

## 📱 **Frontend Integration**

Your frontend can now:
- **Real-time Updates**: Display votes as they come in
- **Device Monitoring**: Track multiple ESP32 terminals
- **Analytics**: Generate voting reports and statistics
- **Audit Trail**: Verify blockchain integrity

## 🚨 **Common Issues & Solutions**

### **ESP32 Can't Connect**
- ✅ Check WiFi credentials
- ✅ Verify server URL is accessible
- ✅ Check firewall settings

### **Votes Not Reaching Backend**
- ✅ Verify endpoint path (`/api/vote`)
- ✅ Check HTTP method (must be POST)
- ✅ Verify Content-Type header

### **Authentication Failures**
- ✅ Check API key format
- ✅ Verify authorization headers
- ✅ Check backend authentication logic

## 🔄 **Next Steps**

1. **Test Basic Integration**: Get simple vote flow working
2. **Add Security**: Implement API keys, HTTPS
3. **Customize Data**: Add election IDs, candidate choices
4. **Scale Up**: Deploy multiple ESP32 terminals
5. **Monitor & Optimize**: Add real-time monitoring

---

**Need Help?** Check the `INTEGRATION_GUIDE.md` for detailed troubleshooting and advanced features.

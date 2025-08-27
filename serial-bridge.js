// Node.js Serial Bridge for ESP32 Biometric Oracle
// Run with: node serial-bridge.js
// Required packages: express serialport @serialport/parser-readline ws cors

const express = require('express');
const cors = require('cors');
const { SerialPort } = require('serialport'); // v13
const { ReadlineParser } = require('@serialport/parser-readline');
const WebSocket = require('ws');

const SERIAL_PORT = 'COM1'; // Your device port
const BAUD_RATE = 115200;

const port = new SerialPort({ path: SERIAL_PORT, baudRate: BAUD_RATE });
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

const app = express();
app.use(cors());
app.use(express.json());

// Generic helper to send action to device and wait for JSON reply
function requestBiometric(action, voterId, timeoutMs = 5000) {
  return new Promise((resolve, reject) => {
    let finished = false;

    const onData = (data) => {
      if (finished) return;
      finished = true;
      clearTimeout(timer);
      try {
        const obj = JSON.parse(data);
        resolve(obj);
      } catch (e) {
        reject(new Error('Invalid JSON from device'));
      }
    };

    const timer = setTimeout(() => {
      if (finished) return;
      finished = true;
      parser.removeListener('data', onData);
      reject(new Error('Timeout waiting for device'));
    }, timeoutMs);

    parser.once('data', onData);

    const payload = voterId ? `${action.toUpperCase()} ${voterId}\n` : `${action.toUpperCase()}\n`;
    port.write(payload, (err) => {
      if (err && !finished) {
        finished = true;
        clearTimeout(timer);
        parser.removeListener('data', onData);
        reject(err);
      }
    });
  });
}

// HTTP endpoint
app.post('/biometric', async (req, res) => {
  const { action, voterId } = req.body;
  if (!['ENROLL_BIOMETRIC', 'VERIFY_BIOMETRIC'].includes(action?.toUpperCase())) {
    return res.status(400).json({ success: false, error: 'Invalid action' });
  }
  if (!voterId || voterId.trim() === '') {
    return res.status(400).json({ success: false, error: 'voterId is required' });
  }
  try {
    const result = await requestBiometric(action, voterId, 8000);
    res.json({ success: true, action, payload: result });
  } catch (e) {
    res.status(500).json({ success: false, error: e.message });
  }
});

// Start server and attach WebSocket
const server = app.listen(5000, () => console.log('Serial bridge listening on port 5000'));
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
  ws.on('message', async (message) => {
    let action, voterId;
    try {
      const parsed = JSON.parse(message);
      action = parsed.action;
      voterId = parsed.voterId;
    } catch (_) {
      action = String(message).trim();
    }

    if (!['ENROLL_BIOMETRIC', 'VERIFY_BIOMETRIC'].includes(action?.toUpperCase())) {
      return ws.send(JSON.stringify({ type: 'error', error: 'Unknown action' }));
    }
    if (!voterId || voterId.trim() === '') {
      return ws.send(JSON.stringify({ type: 'error', error: 'voterId is required' }));
    }

    try {
      const result = await requestBiometric(action, voterId, 8000);
      ws.send(JSON.stringify({ type: 'biometric_result', action, payload: result }));
    } catch (e) {
      ws.send(JSON.stringify({ type: 'error', error: e.message }));
    }
  });
});

port.on('error', (err) => console.error('Serial port error:', err.message));

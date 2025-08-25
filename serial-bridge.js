// Node.js Serial Bridge for ESP32 Biometric Oracle
// Place this file in your project root and run with: node serial-bridge.js
// Required packages: express serialport @serialport/parser-readline ws cors

const express = require('express');
const cors = require('cors');
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
const WebSocket = require('ws');

const SERIAL_PORT = 'COM3'; // set to your device port
const BAUD_RATE = 115200;

const port = new SerialPort(SERIAL_PORT, { baudRate: BAUD_RATE });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

const app = express();
app.use(cors());
app.use(express.json());

// helper: send AUTHENTICATE and wait for one line of JSON reply (with timeout)
function requestAuthenticate(timeoutMs = 3000) {
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
    port.write('AUTHENTICATE\n', (err) => {
      if (err && !finished) {
        finished = true;
        clearTimeout(timer);
        parser.removeListener('data', onData);
        reject(err);
      }
    });
  });
}

// HTTP endpoint used by backend/frontend directly
app.post('/authenticate', async (req, res) => {
  try {
    const result = await requestAuthenticate(5000);
    res.json(result);
  } catch (e) {
    res.status(500).json({ success: false, error: e.message });
  }
});

// start HTTP server and attach WebSocket server to it
const server = app.listen(5000, () => console.log('Serial bridge listening on port 5000'));
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
  ws.on('message', async (message) => {
    // accept either plain 'AUTHENTICATE' or JSON { action: 'authenticate' }
    let action = null;
    try {
      const parsed = JSON.parse(message);
      action = parsed && parsed.action;
    } catch (_) {
      action = String(message).trim();
    }

    if (String(action).toUpperCase() === 'AUTHENTICATE') {
      try {
        const result = await requestAuthenticate(5000);
        ws.send(JSON.stringify({ type: 'authenticate_result', payload: result }));
      } catch (e) {
        ws.send(JSON.stringify({ type: 'error', error: e.message }));
      }
    } else {
      ws.send(JSON.stringify({ type: 'error', error: 'Unknown action' }));
    }
  });
});

// graceful handling for serial port errors
port.on('error', (err) => {
  console.error('Serial port error:', err.message);
});

// serial-bridge.js (CommonJS version)
const { SerialPort } = require("serialport");
const { WebSocketServer } = require("ws");

const SERIAL_PORT = "COM8";   // Replace with your ESP32 COM port
const SERIAL_BAUD = 115200;

// ----- Setup Serial Port -----
const port = new SerialPort({ path: SERIAL_PORT, baudRate: SERIAL_BAUD });
port.on("open", () => console.log(`Serial port ${SERIAL_PORT} opened at ${SERIAL_BAUD}`));
port.on("error", (err) => console.error("Serial port error:", err));

// ----- WebSocket Server -----
const wss = new WebSocketServer({ port: 5001 });
console.log("WebSocket bridge listening on ws://localhost:5001");

// Store connected clients
const clients = new Set();

wss.on("connection", (ws) => {
  console.log("Frontend connected via WebSocket");
  clients.add(ws);

  ws.on("message", (msg) => {
    try {
      const data = JSON.parse(msg.toString());
      console.log("Received from frontend:", data);

      if (data.type === "enroll" || data.type === "verify") {
        // Forward enroll/verify command to ESP32
        const command = JSON.stringify({
          action: data.type.toUpperCase() + "_BIOMETRIC",
          voterId: data.voterId
        }) + "\n";
        port.write(command, (err) => {
          if (err) console.error("Error writing to serial port:", err);
          else console.log(`Sent to ESP32: ${command.trim()}`);
        });
      } else if (data.type === "clear") {
        // Forward clear DB command to ESP32
        const command = JSON.stringify({ action: "CLEAR_BIOMETRIC_DB" }) + "\n";
        port.write(command, (err) => {
          if (err) console.error("Error writing to serial port:", err);
          else console.log(`Sent to ESP32: ${command.trim()}`);
        });
      } else {
        console.warn("Unknown command type:", data.type);
      }
    } catch (err) {
      console.error("Failed to parse message from frontend:", err);
    }
  });

  ws.on("close", () => {
    clients.delete(ws);
    console.log("Frontend disconnected");
  });
});

// ----- Listen for ESP32 Responses -----
let serialBuffer = "";

port.on("data", (data) => {
  serialBuffer += data.toString();

  // Split on newline
  let lines = serialBuffer.split("\n");
  serialBuffer = lines.pop(); // keep last incomplete chunk

  lines.forEach((line) => {
    line = line.trim();
    if (!line) return;

    console.log("Received from ESP32:", line);

    // Validate JSON before sending
    try {
      const json = JSON.parse(line);
      clients.forEach((ws) => {
        ws.send(JSON.stringify(json));
      });
    } catch (err) {
      console.error("Invalid JSON from ESP32:", line);
    }
  });
});

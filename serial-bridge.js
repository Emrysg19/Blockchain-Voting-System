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
        // Forward command to ESP32
        const command = JSON.stringify({ action: data.type.toUpperCase() + "_BIOMETRIC", voterId: data.voterId }) + "\n";
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
port.on("data", (data) => {
  const lines = data.toString().split("\n").filter(Boolean);
  lines.forEach((line) => {
    console.log("Received from ESP32:", line.trim());

    // Broadcast to all connected frontend clients
    clients.forEach((ws) => {
      try {
        ws.send(line.trim());
      } catch (err) {
        console.error("Failed to send to frontend:", err);
      }
    });
  });
});

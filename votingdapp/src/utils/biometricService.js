// biometricService.js
// Utility functions to interact with biometric verification service
const WS_URL = import.meta.env.VITE_BIOMETRIC_WS_URL || "ws://localhost:5001";
const HTTP_URL = import.meta.env.VITE_BIOMETRIC_HTTP_URL || "http://localhost:5000";

function timeoutPromise(ms) {
  return new Promise((_, reject) => setTimeout(() => reject(new Error("Biometric verification timed out")), ms));
}

export function getBiometricConfig() {
    return { wsUrl: WS_URL, httpUrl: HTTP_URL };
}

export async function verifyBiometric(voterId, options = {}) {
  if (!voterId || voterId.trim().length === 0) {
    throw new Error("Voter ID is required for biometric verification");
  }

  const timeoutMs = options?.timeoutMs || 30000;

  return new Promise((resolve, reject) => {
    let settled = false;
    const ws = new WebSocket(WS_URL);

    let timer = setTimeout(() => {
      if (!settled) {
        settled = true;
        ws.close();
        reject(new Error("Biometric verification timed out"));
      }
    }, timeoutMs);

    ws.onopen = () => {
      ws.send(JSON.stringify({ type: "verify", voterId }));
    };

    ws.onmessage = (event) => {
      if (settled) return;
      try {
        const data = JSON.parse(event.data);
        if (data.type === "verification_result" && data.voterId === voterId) {
          settled = true;
          clearTimeout(timer);
          ws.close();
          resolve({ success: data.status === "VERIFIED", payload: data });
        }
      } catch {
        // ignore non-JSON frames
      }
    };

    ws.onerror = (err) => {
      if (!settled) {
        settled = true;
        clearTimeout(timer);
        ws.close();
        reject(new Error("Biometric WebSocket error"));
      }
    };

    ws.onclose = () => {
      if (!settled) {
        settled = true;
        clearTimeout(timer);
        reject(new Error("Biometric WebSocket closed before result"));
      }
    };
  });
}

export async function enrollBiometric(voterId, options = {}) {
  if (!voterId || voterId.trim().length === 0) {
    throw new Error("Voter ID is required for enrollment");
  }

  const timeoutMs = options?.timeoutMs || 30000;

  return new Promise((resolve, reject) => {
    let settled = false;
    const ws = new WebSocket(WS_URL);

    let timer = setTimeout(() => {
      if (!settled) {
        settled = true;
        ws.close();
        reject(new Error("Biometric enrollment timed out"));
      }
    }, timeoutMs);

    ws.onopen = () => {
      ws.send(JSON.stringify({ type: "enroll", voterId }));
    };

    ws.onmessage = (event) => {
      if (settled) return;
      try {
        const data = JSON.parse(event.data);
        if (data.type === "enroll" && data.voterId === voterId) {
          settled = true;
          clearTimeout(timer);
          ws.close();
          resolve({ success: data.status === "ENROLLED", payload: data });
        }
      } catch {
        // ignore non-JSON frames
      }
    };

    ws.onerror = (err) => {
      if (!settled) {
        settled = true;
        clearTimeout(timer);
        ws.close();
        reject(new Error("Biometric WebSocket error"));
      }
    };

    ws.onclose = () => {
      if (!settled) {
        settled = true;
        clearTimeout(timer);
        reject(new Error("Biometric WebSocket closed before result"));
      }
    };
  });
}

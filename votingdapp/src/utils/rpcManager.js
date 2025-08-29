export class RPCManager {
  constructor() {
    // Only use custom RPC endpoints
    const customRpc = import.meta.env.POLYGON_RPC_URL || import.meta.env.VITE_INFURA_RPC;
    const customWssRpc = import.meta.env.VITE_INFURA_WSS_RPC;

    if (!customRpc) {
      throw new Error("Custom RPC endpoint not configured in environment variables.");
    }

    this.rpcEndpoints = [customRpc]; // only custom RPC
    this.wssEndpoint = customWssRpc || null;

    this.currentEndpointIndex = 0;
    this.maxRetries = 3;
    this.retryDelay = 1000;

    console.log(`🔗 Custom RPC endpoint configured: ${customRpc}`);
    if (customWssRpc) {
      console.log(`🔗 WebSocket RPC endpoint configured: ${customWssRpc}`);
    }
  }

  getCurrentEndpoint() {
    return this.rpcEndpoints[this.currentEndpointIndex];
  }

  switchToNextEndpoint() {
    // No switching; only one endpoint
    console.warn("No fallback endpoints configured, cannot switch RPC endpoint.");
    return this.getCurrentEndpoint();
  }

  async testEndpoint(url) {
    try {
      const response = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          jsonrpc: '2.0',
          method: 'eth_blockNumber',
          params: [],
          id: 1
        }),
        signal: AbortSignal.timeout(5000)
      });

      if (!response.ok) return false;

      const data = await response.json();
      return !!data.result && !isNaN(parseInt(data.result, 16));
    } catch (err) {
      console.warn(`RPC test failed for ${url}: ${err.message}`);
      return false;
    }
  }

  async findWorkingEndpoint() {
    const endpoint = this.getCurrentEndpoint();
    console.log(`🔍 Testing custom RPC endpoint: ${endpoint}`);
    if (await this.testEndpoint(endpoint)) {
      console.log(`✅ Custom RPC endpoint is working: ${endpoint}`);
      return endpoint;
    }
    throw new Error(`❌ Custom RPC endpoint failed: ${endpoint}`);
  }

  async executeWithRetry(operation, maxRetries = this.maxRetries) {
    let lastError;
    for (let attempt = 1; attempt <= maxRetries; attempt++) {
      try {
        return await operation();
      } catch (err) {
        lastError = err;
        console.warn(`RPC error on attempt ${attempt}: ${err.message}`);
        await this.delay(this.retryDelay * attempt);
      }
    }
    throw lastError;
  }

  delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  getWssEndpoint() {
    return this.wssEndpoint;
  }

  getConfigurationInfo() {
    return {
      customRpc: this.rpcEndpoints[0],
      wssEndpoint: this.wssEndpoint
    };
  }
}

// Singleton instance
export const rpcManager = new RPCManager();

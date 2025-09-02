# Project Setup Guide

## 1. Clone the Repository
```bash
git clone <YOUR_REPO_URL>
cd <YOUR_REPO_FOLDER>

## 2. install node.js and npm dependencies then run
npm install

## For ESP32 FIRMWARE
# Make sure you have PlatformIO installed
pio update
pio run

## CONFIGURE ENVIRONMENT VARIABLES .ENV
VITE_BIOMETRIC_WS_URL=ws://localhost:5001
VITE_BIOMETRIC_HTTP_URL=http://localhost:5000
POLYGON_RPC_URL=https://polygon-mainnet.infura.io/v3/YOUR_INFURA_KEY
VITE_INFURA_WSS_RPC=wss://polygon-mainnet.infura.io/ws/v3/YOUR_INFURA_KEY
VITE_CONTRACT_ADDRESS=<YOUR_CONTRACT_ADDRESS>

## START FRONTEND
npm run dev

## COMMON COMMANDS
# Check ESP32 logs
pio device monitor

# Check WebSocket logs
wscat -c ws://localhost:5001

# Git workflow
git checkout main
git pull origin main
git merge restart-dev

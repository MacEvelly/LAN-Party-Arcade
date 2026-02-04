# MiniSD - SD Card Content

This folder contains files that should be copied to your physical SD card.

## Contents

- `config.json` - Optional WiFi configuration (SSID, password, hostname)
- Future: Game folders (poker, trivia, etc.)
- Future: Shared framework files

## Usage

1. Copy the contents of this folder to the root of your formatted SD card (FAT32)
2. Insert SD card into ESP32-2432S028
3. Power on - ESP32 will read config and serve game files

## config.json Options

```json
{
  "wifiSSID": "LAN_Party_SD",      // WiFi network name (default: "LAN_Party_Arcade")
  "wifiPassword": "",               // WiFi password (empty = open network)
  "hostname": "play",               // mDNS hostname (default: "play" -> play.local)
  "headerBMP": "Header.bmp",        // Header image filename (default: "Header.bmp")
  "maxConnections": 20              // Max simultaneous WiFi clients
}
```

**Header Image Requirements:**
- **Filename**: Set via `headerBMP` in config.json (e.g., "Header.bmp", "logo.bmp")
- **Dimensions**: 200 pixels wide × 64 pixels tall
- **Format**: 24-bit BMP (uncompressed)
- **Location**: Root of SD card (e.g., `/Header.bmp`)
- If file is missing, displays "JOIN GAME" text instead

**Display Character Limits:****
- SSID: 15 characters (truncated with ".." if longer)
- Password: 15 characters shown as asterisks (shows ".." if longer)
- URL: 15 characters including ".local" (truncated with ".." if longer)

**Hostname Feature:**
- Sets the mDNS address: `http://<hostname>.local`
- Generates QR code with this URL on the display
- Useful for running multiple devices or custom branding

## File Structure on SD Card

```
/
├── config.json          (WiFi + network settings)
├── poker/               (game folder - future)
├── trivia/              (game folder - future)
└── _framework/          (shared React framework - future)
```

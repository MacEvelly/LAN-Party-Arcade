# MiniSD - SD Card Content

This folder contains files that should be copied to your physical SD card.

## Contents

- `config.json` - Optional WiFi configuration
- Future: Game folders (poker, trivia, etc.)
- Future: Shared framework files

## Usage

1. Copy the contents of this folder to the root of your formatted SD card (FAT32)
2. Insert SD card into ESP32-2432S028
3. Power on - ESP32 will read config and serve game files

## File Structure on SD Card

```
/
├── config.json          (optional WiFi settings)
├── poker/               (game folder - future)
├── trivia/              (game folder - future)
└── _framework/          (shared React framework - future)
```

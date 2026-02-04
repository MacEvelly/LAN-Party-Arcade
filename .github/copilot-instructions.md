# Agent Instructions - LAN-Party-Arcade

## Project Overview

This project transforms the ESP32-2432S028 into a multiplayer game server using the "Nintendo cartridge" mental model:

- **ESP32 = NES Console**: Boots games, routes messages, serves files - intentionally "dumb"
- **SD Card = Game Cartridge**: Hot-swappable, self-contained games
- **Phones = Controllers + Personal Displays**: Each player's private interface
- **Client-Heavy Architecture**: Game logic runs on phones, ESP32 only relays WebSocket messages

**Core Philosophy:**
- Keep ESP32 simple and reliable (pure message relay)
- Push complexity to clients (they're faster)
- Design modular: everything is swappable/upgradeable
- Code for V2.0: touch interactions, LVGL rendering, advanced features planned but not blocking V1.0

**Current Version:** V1.0 (Foundation)
- Focus: Get core relay working
- Display: System info only (no game rendering yet)
- Touch: Planned for V2.0

**Technologies:**
- Platform: PlatformIO + Arduino framework
- Board: ESP32 Dev Module (`esp32dev`)
- Client: React + Zustand + WebSockets
- Storage: SD card (FAT32)

**Project Structure:**
- `src/` - ESP32 firmware code
- `MiniSD/` - SD card content (copy to physical SD card)
  - `config.json` - WiFi configuration
  - Game folders (future)
  - Framework files (future)

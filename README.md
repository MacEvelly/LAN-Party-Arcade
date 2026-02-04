# 🎮 LAN Party Arcade

**Transform an ESP32 into a multiplayer game server using the "Nintendo cartridge" philosophy.**

[![Version](https://img.shields.io/badge/version-1.0-blue)]() [![License](https://img.shields.io/badge/license-MIT-green)]() [![Platform](https://img.shields.io/badge/platform-ESP32-red)]()

---

## 🌟 Core Concept

**ESP32 = NES Console** | **SD Card = Game Cartridge** | **Phones = Controllers**

The ESP32 acts as a "dumb relay" - it boots games from SD cards and routes messages between players. All game logic runs on the phones, making games easy to develop and instantly hot-swappable.

### Features

- ✅ **Hot-swappable games** via SD card (no firmware updates)
- ✅ **WiFi Access Point** with QR code connection
- ✅ **WebSocket relay** for real-time multiplayer (port 81)
- ✅ **Web server** serves games from SD card (port 80)
- ✅ **mDNS support** - access via `http://play.local`
- ✅ **Client-heavy architecture** - ESP32 just relays messages
- ✅ **TFT display** shows WiFi QR codes and connection info
- ✅ **Configurable** via JSON on SD card

---

## 📸 Quick Look

![Display showing QR codes](docs/images/display_qr.jpg) *(Add your photo)*

**What you see:**
- Logo at top (customizable BMP image)
- WiFi QR code (scan to connect)
- URL QR code (scan to open game)
- Network details (SSID, password, URL)

---

## 🚀 Quick Start

### Hardware Needed
- **ESP32-2432S028** ("Cheap Yellow Display")
- **MicroSD card** (FAT32, any size)
- **USB cable** (power/programming)

### Setup (3 Steps)

1. **Flash firmware** (PlatformIO)
   ```bash
   git clone https://github.com/yourusername/LAN-Party-Arcade.git
   cd LAN-Party-Arcade
   pio run --target upload
   ```

2. **Prepare SD card**
   - Format as FAT32
   - Copy everything from `MiniSD/` folder to SD card root
   - Edit `config.json` with your WiFi name

3. **Power on & play**
   - Insert SD card into ESP32
   - Power on device
   - Scan QR code on display to connect
   - Open URL to play games

---

## 🎮 Playing Games

1. **Connect to WiFi** - Scan QR code on ESP32 display
2. **Open browser** - Navigate to `http://play.local/`
3. **Choose game** - Select from landing page
4. **Multi-player** - Others join same URL, actions sync in real-time

**Example:** Dice Roller
- Open `http://play.local/games/_example_dice_roller/`
- Click "Roll Dice"
- Everyone sees your roll instantly

---

## 🛠️ Development

### Architecture Philosophy

**ESP32 = Message Router (Dumb)**
- No game logic
- Just forwards WebSocket messages
- Never needs firmware updates for new games

**Phones = Game Engine (Smart)**
- All game rules run client-side
- ESP32 relays actions between players
- Games are just HTML/CSS/JS files

### Creating Games

1. **Copy template**
   ```bash
   cp -r MiniSD/games/_example_dice_roller MiniSD/games/my_game
   ```

2. **Edit `index.html`** - Change game logic
   ```javascript
   // ESP32 broadcasts this to all other players
   ws.send(JSON.stringify({
     type: 'game_action',
     uuid: playerUUID,
     data: { /* your game state */ }
   }));
   ```

3. **Test** - Copy to SD card, access at `http://play.local/games/my_game/`

**See:** [`MiniSD/games/_example_dice_roller/README.md`](MiniSD/games/_example_dice_roller/README.md) for full guide.

---

## 📂 Project Structure

```
LAN-Party-Arcade/
├── src/
│   └── main.cpp           # ESP32 firmware (WebSocket relay, file server)
├── MiniSD/                # Everything that goes on SD card
│   ├── config.json        # WiFi settings
│   ├── index.html         # Landing page
│   ├── test.html          # WebSocket test tool
│   ├── Header.bmp         # Logo (optional)
│   └── games/
│       └── _example_dice_roller/  # Template game
├── docs/                  # Documentation
├── platformio.ini         # Build config
└── README.md              # This file
```

---

## ⚙️ Configuration

Edit `MiniSD/config.json`:

```json
{
  "wifiSSID": "MyLANParty",      // Custom WiFi name
  "wifiPassword": "",             // Leave empty for open network
  "hostname": "play",             // URL becomes play.local
  "headerBMP": "Header.bmp",      // Logo file (200×64px BMP)
  "maxConnections": 20            // Max players
}
```

Copy entire `MiniSD/` folder to SD card root.

**Full guide:** [`MiniSD/SD_CARD_GUIDE.md`](MiniSD/SD_CARD_GUIDE.md)

---

## 📖 Documentation

- **[Hardware Setup](docs/HARDWARE.md)** - Pinouts, wiring, board specs
- **[SD Card Guide](MiniSD/SD_CARD_GUIDE.md)** - File structure, config options
- **[Game Development](MiniSD/games/_example_dice_roller/README.md)** - Build your first game
- **[Architecture](docs/ARCHITECTURE.md)** - Design philosophy, WebSocket protocol
- **[Contributing](CONTRIBUTING.md)** - How to contribute

---

## 🔧 Technical Details

### Hardware
- **Board:** ESP32-2432S028 (ESP32 Dev Module)
- **Display:** 2.8" ILI9341 TFT (240×320px, portrait)
- **Touch:** XPT2046 resistive (V2.0 feature)
- **Storage:** MicroSD card (FAT32)

### Network
- **WiFi:** Access Point mode, configurable SSID
- **DNS:** Wildcard server (any domain → ESP32 IP)
- **HTTP:** Port 80 (file server)
- **WebSocket:** Port 81 (game relay)
- **mDNS:** `play.local` (configurable)

### Libraries
- `TFT_eSPI` - Display driver
- `ArduinoJson` - Config parsing
- `WebSockets` - Real-time messaging
- `QRCode` - QR code generation

---

## 🎯 Roadmap

### V1.0 (Current)
- [x] WiFi AP with QR code
- [x] WebSocket relay
- [x] File server from SD
- [x] Portrait display with dual QR codes
- [x] Configurable via config.json
- [x] Example dice roller game

### V2.0 (Future)
- [ ] Touch screen interaction
- [ ] LVGL UI rendering
- [ ] Advanced game templates (poker, trivia)
- [ ] React framework for games
- [ ] Game state persistence
- [ ] Player authentication

---

## 🤝 Contributing

Contributions welcome! See [`CONTRIBUTING.md`](CONTRIBUTING.md) for guidelines.

**Ways to help:**
- 🎮 Create new game templates
- 📝 Improve documentation
- 🐛 Report/fix bugs
- 💡 Suggest features

---

## 📜 License

MIT License - See [`LICENSE`](LICENSE) for details.

---

## 🙏 Acknowledgments

- **ESP32-2432S028 Community** - Hardware documentation
- **TFT_eSPI** - Display library
- **Nintendo** - For the "cartridge" inspiration 🎮

---

## 📬 Support

- **Issues:** [GitHub Issues](https://github.com/yourusername/LAN-Party-Arcade/issues)
- **Discussions:** [GitHub Discussions](https://github.com/yourusername/LAN-Party-Arcade/discussions)
- **Email:** your.email@example.com

---

**Made with ❤️ for local multiplayer gaming**

*No internet required. No app stores. Just plug, play, and party.*

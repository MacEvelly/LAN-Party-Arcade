# MiniSD - SD Card Content

This folder contains **everything** that goes on your physical SD card. Copy the entire contents to the root of your SD card.

## 📂 File Structure

```
SD Card Root/
├── config.json                    # WiFi and system configuration
├── index.html                     # Landing page (auto-served at root)
├── test.html                      # WebSocket testing interface
├── Header.bmp                     # Logo image (200×64px, optional)
└── games/                         # All games go here
    ├── _example_dice_roller/      # Template game (underscore = example)
    │   ├── index.html
    │   └── README.md
    ├── poker/                     # Your real games (future)
    └── trivia/                    # (future)
```

## 🎯 Required Files

### **config.json** (System Settings)
```json
{
  "wifiSSID": "LAN_Party_SD",      // WiFi network name
  "wifiPassword": "",               // WiFi password (empty = open)
  "hostname": "play",               // URL hostname (play.local)
  "headerBMP": "Header.bmp",        // Logo filename
  "maxConnections": 20              // Max WiFi clients
}
```

**Configuration Options:**
- **wifiSSID**: Custom network name (max 15 chars on display)
- **wifiPassword**: Leave empty for open network, or set password
- **hostname**: Changes URL to `http://<hostname>.local`
- **headerBMP**: Logo file (200×64px, 24-bit BMP format)
- **maxConnections**: Max simultaneous WiFi connections (1-20)

### **index.html** (Landing Page)
- First page users see when connecting
- Served automatically at `http://play.local/`
- Links to test page and games

### **test.html** (WebSocket Test)
- Development tool for testing real-time messaging
- Shows connection status, message relay
- Open in multiple tabs to simulate players

## 🎨 Optional Files

### **Header.bmp** (Logo Image)
- **Dimensions**: 200 pixels wide × 64 pixels tall
- **Format**: 24-bit BMP (uncompressed)
- **Location**: Root of SD card
- **Fallback**: If missing, displays "JOIN GAME" text

## 🎮 Games Folder

All games live in the `/games/` directory. Each game gets its own subfolder.

### **Example Game Template**
The `_example_dice_roller` folder is a working template showing:
- WebSocket connection setup
- Player UUID management
- Broadcasting game actions
- Receiving other players' updates

**To create a new game:**
1. Copy `_example_dice_roller` folder
2. Rename to your game name (no underscore)
3. Modify `index.html` with your game logic
4. Test at `http://play.local/games/your_game_name/`

### **Game URL Pattern**
```
http://play.local/games/<folder_name>/
```
Examples:
- `http://play.local/games/_example_dice_roller/`
- `http://play.local/games/poker/`
- `http://play.local/games/trivia/`

## 📋 Setup Instructions

### **1. Format SD Card**
- Format: FAT32
- Size: Any (tested with 32GB)

### **2. Copy Files**
```
1. Copy everything in this MiniSD folder to SD card root
2. Edit config.json with your WiFi settings
3. (Optional) Add your Header.bmp logo
4. Eject SD card safely
```

### **3. Insert & Boot**
```
1. Insert SD card into ESP32-2432S028
2. Power on device
3. Join WiFi network (scan QR code on display)
4. Open http://play.local/ or scan URL QR code
```

## 🔧 Display Character Limits

Text shown on ESP32 display has size constraints:

- **SSID**: 15 characters (truncated with "..")
- **Password**: 15 characters (truncated with "..")
- **URL**: 15 characters including ".local" (truncated with "..")

Choose short, memorable values for best display readability.

## 📚 Additional Resources

- **Hardware Setup**: See `/docs/HARDWARE.md` in project root
- **Configuration Guide**: See `/docs/CONFIGURATION.md`
- **Game Development**: See `games/_example_dice_roller/README.md`
- **Architecture**: See `/docs/ARCHITECTURE.md`

## 🐛 Troubleshooting

**SD card not detected:**
- Ensure FAT32 format
- Try different SD card
- Check serial monitor for errors

**Config not loading:**
- Validate JSON syntax at jsonlint.com
- Check file is named exactly `config.json`
- No BOM encoding (use UTF-8 without BOM)

**Logo not showing:**
- Verify 200×64px dimensions
- Must be 24-bit BMP (not JPEG/PNG)
- Check `headerBMP` filename matches config

**Can't connect to WiFi:**
- SSID shown on display is the one to join
- Password field shows actual password (not ***)
- Open network if password field empty

## 💡 Tips

- **Keep backups** - Copy this folder to computer as template
- **Version games** - Use folders like `poker_v1/`, `poker_v2/`
- **Test locally** - Open HTML files in browser before deploying
- **Start simple** - Fork dice roller, don't build complex game first
- **Multiple devices** - SD cards are cheap, make different configs

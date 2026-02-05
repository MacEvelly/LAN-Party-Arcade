# Contributing to LAN Party Arcade

Thank you for your interest in contributing! This project thrives on community involvement. Whether you're fixing bugs, adding features, creating games, or improving documentation, your contributions are welcome.

---

## 🎯 Ways to Contribute

### 1. Create Games
The easiest way to contribute! Create multiplayer games that run in the browser.

**Start here:**
- Copy [MiniSD/games/_example_dice_roller/](../MiniSD/games/_example_dice_roller/)
- Modify the HTML/JavaScript
- Test locally in your browser
- Submit a PR with your game

**Game Requirements:**
- Must use WebSocket for multiplayer
- Must generate UUID for player identification
- Should work on mobile browsers
- Include README.md explaining gameplay

### 2. Improve Firmware
Help make the ESP32 code better, faster, or more reliable.

**Areas needing help:**
- Touch screen functionality (V2.0)
- Performance optimizations
- Additional network protocols
- Display enhancements (LVGL integration)

### 3. Write Documentation
Help others understand and use the project.

**Documentation needs:**
- Tutorial videos
- Troubleshooting guides
- Hardware assembly instructions
- Game development tutorials

### 4. Report Bugs
Found something broken? Let us know!

### 5. Suggest Features
Have ideas for improvements? Open an issue!

---

## 🚀 Getting Started

### Prerequisites

**For Firmware Development:**
- VS Code with PlatformIO extension
- ESP32-2432S028 board (or compatible)
- USB-C cable
- Basic C++ knowledge

**For Game Development:**
- Text editor (VS Code, Sublime, etc.)
- Web browser (Chrome recommended)
- Basic HTML/JavaScript knowledge
- MicroSD card (for testing on hardware)

### Setting Up Development Environment

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/lan-party-arcade.git
   cd lan-party-arcade
   ```

2. **Install dependencies (firmware):**
   ```bash
   # PlatformIO will auto-install libraries
   pio run
   ```

3. **Test locally (games):**
   ```bash
   cd MiniSD
   python -m http.server 8000
   # Open http://localhost:8000/games/dice_roller/
   ```

---

## 📋 Development Workflow

### For Firmware Changes

1. **Create a feature branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes:**
   - Follow existing code style
   - Add comments for complex logic
   - Update relevant documentation

3. **Test thoroughly:**
   ```bash
   pio run              # Compile
   pio run -t upload    # Upload to board
   pio device monitor   # Check serial output
   ```

4. **Commit with clear messages:**
   ```bash
   git add .
   git commit -m "feat: Add room system to WebSocket relay"
   ```

5. **Push and create PR:**
   ```bash
   git push origin feature/your-feature-name
   ```

### For Game Development

1. **Copy example game:**
   ```bash
   cp -r MiniSD/games/_example_dice_roller MiniSD/games/my_game
   ```

2. **Develop and test:**
   - Edit HTML/JavaScript
   - Test in browser with `python -m http.server`
   - Test on actual hardware (copy to SD card)

3. **Add documentation:**
   - Create `README.md` in game folder
   - Explain rules and controls
   - List any requirements

4. **Submit PR:**
   - Include screenshots/GIFs
   - Test with multiple players
   - Verify on mobile devices

---

## 🎨 Code Style Guidelines

### C++ (Firmware)

**Naming Conventions:**
```cpp
// Classes: PascalCase
class WiFiManager { };

// Functions: camelCase
void startAccessPoint();

// Variables: camelCase
String actualSSID;

// Constants: UPPER_SNAKE_CASE
#define SD_CS 5

// Static members: camelCase
static WebServer server;
```

**File Organization:**
```cpp
// header.h
#ifndef HEADER_H
#define HEADER_H

// Includes
#include <Arduino.h>

// Class declaration
class MyClass {
public:
    static void publicMethod();
private:
    static void privateMethod();
    static int memberVariable;
};

#endif
```

**Code Style:**
- **Indentation:** 2 spaces (no tabs)
- **Braces:** K&R style (opening brace on same line)
- **Line length:** 100 characters max
- **Comments:** Explain WHY, not WHAT

**Example:**
```cpp
// Good: Explains reasoning
// Use HSPI for display to avoid conflict with SD card on VSPI
tft.init();

// Bad: States the obvious
// Initialize display
tft.init();
```

### JavaScript (Games)

**Naming Conventions:**
```javascript
// Functions: camelCase
function rollDice() { }

// Variables: camelCase
const playerUUID = generateUUID();

// Constants: UPPER_SNAKE_CASE
const MAX_PLAYERS = 20;

// Classes: PascalCase
class GameState { }
```

**Code Style:**
- **Indentation:** 2 spaces
- **Quotes:** Single quotes for strings
- **Semicolons:** Always use them
- **Modern JS:** Use ES6+ features (const, let, arrow functions)

**Example:**
```javascript
// Good: Modern, clean
const rollDice = () => {
  const result = Math.floor(Math.random() * 6) + 1;
  sendMessage({ type: 'dice_roll', result });
};

// Avoid: Old style
var rollDice = function() {
  var result = Math.floor(Math.random() * 6) + 1;
  sendMessage({ type: 'dice_roll', result: result });
};
```

---

## 🧪 Testing Guidelines

### Firmware Testing

**Required tests before PR:**
1. ✅ Compiles without warnings
2. ✅ ESP32 boots and shows connection screen
3. ✅ WiFi AP starts successfully
4. ✅ Can connect from phone
5. ✅ WebSocket messages relay correctly
6. ✅ SD card reads files
7. ✅ No memory leaks (check `ESP.getFreeHeap()`)

**Testing checklist:**
```cpp
// Add to Serial output for debugging
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
Serial.printf("WiFi clients: %d\n", WiFi.softAPgetStationNum());
Serial.printf("WS clients: %d\n", WebSocketRelay::getClientCount());
```

### Game Testing

**Required tests before PR:**
1. ✅ Works in Chrome desktop
2. ✅ Works in Chrome mobile
3. ✅ Works with 2+ players simultaneously
4. ✅ Handles disconnections gracefully
5. ✅ No console errors
6. ✅ Responsive on different screen sizes

**Browser DevTools checklist:**
- Check Console for errors
- Monitor Network tab for WebSocket messages
- Test with throttled connection (3G simulation)
- Verify on iPhone Safari and Android Chrome

---

## 📦 Module Development

### Adding a New Module

1. **Choose appropriate location:**
   ```
   src/
   ├── storage/    ← Data persistence
   ├── network/    ← Network services
   ├── display/    ← Display & UI
   └── utils/      ← Helper functions
   ```

2. **Create header file:**
   ```cpp
   // src/category/my_module.h
   #ifndef MY_MODULE_H
   #define MY_MODULE_H
   
   class MyModule {
   public:
       static void init();
       static void process();
   };
   
   #endif
   ```

3. **Create implementation:**
   ```cpp
   // src/category/my_module.cpp
   #include "my_module.h"
   
   void MyModule::init() {
       // Initialization code
   }
   
   void MyModule::process() {
       // Main logic
   }
   ```

4. **Update main.cpp:**
   ```cpp
   #include "category/my_module.h"
   
   void setup() {
       MyModule::init();
   }
   
   void loop() {
       MyModule::process();
   }
   ```

5. **Document in ARCHITECTURE.md**

### Module Design Principles

- **Single Responsibility:** One module = one purpose
- **No Circular Dependencies:** Use forward declarations
- **Static Classes:** Singletons via static methods
- **Clear Interfaces:** Public methods documented
- **Private Implementation:** Hide internal details

---

## 🐛 Bug Reports

### Creating Good Bug Reports

**Use the bug report template** (see `.github/ISSUE_TEMPLATE/bug_report.md`)

**Include:**
1. **Clear title:** "WebSocket disconnects after 30 seconds"
2. **Steps to reproduce:** Numbered list
3. **Expected behavior:** What should happen
4. **Actual behavior:** What actually happens
5. **Environment:**
   - Board: ESP32-2432S028
   - Firmware version: 1.0.2
   - Library versions (from platformio.ini)
   - Browser/OS (for games)
6. **Serial output:** Copy relevant logs
7. **Screenshots/videos:** If applicable

**Example:**
```markdown
## Bug: Display shows corrupted QR codes

**Steps to reproduce:**
1. Flash firmware v1.0.2
2. Power on ESP32
3. Observe connection screen

**Expected:** Two clear QR codes side-by-side
**Actual:** QR codes overlap and are unreadable

**Environment:**
- Board: ESP32-2432S028 Rev 2.1
- Firmware: v1.0.2
- TFT_eSPI: 2.5.43

**Serial output:**
```
Display initialized!
QR size: 33 modules
Drawing at X=15, Y=80
```

**Photo attached:** [screenshot.jpg]
```

---

## 💡 Feature Requests

### Proposing New Features

**Use the feature request template** (see `.github/ISSUE_TEMPLATE/feature_request.md`)

**Include:**
1. **Problem statement:** What need does this address?
2. **Proposed solution:** How would it work?
3. **Alternatives considered:** Other approaches
4. **Implementation notes:** Technical details (optional)
5. **Breaking changes:** Impact on existing code

**Example:**
```markdown
## Feature Request: Game Rooms

**Problem:**
Currently all players are in one game. With 20 players, 
games become chaotic. Need way to split into smaller groups.

**Proposed Solution:**
Add room system to WebSocket relay:
- Players join specific room (URL parameter: ?room=abc)
- Messages only relay within room
- Display shows active rooms

**Alternatives:**
1. Multiple ESP32 boards (expensive)
2. Client-side filtering (unreliable)

**Implementation:**
- Modify websocket_server.cpp
- Add room tracking to clients map
- Update relay logic to filter by room

**Breaking Changes:**
None - backward compatible (default room for existing games)
```

---

## 📄 Documentation

### Documentation Standards

**For code:**
- Public functions: Document purpose, parameters, return value
- Complex logic: Explain approach with comments
- Modules: Create README.md in module folder

**For games:**
- Every game needs README.md
- Explain rules in simple terms
- Include controls and requirements

**Example (game README):**
```markdown
# Rock Paper Scissors

A classic RPS game for 2-6 players.

## How to Play
1. Wait for all players to join
2. Make your selection (rock/paper/scissors)
3. Results shown when all players submit
4. Winner gets 1 point

## Controls
- Tap icon to select
- Results auto-display after 10 seconds

## Requirements
- 2-6 players
- Modern browser with WebSocket support
```

---

## 🎁 Game Submission Guidelines

### Game Checklist

Before submitting a game PR:

- [ ] Game works with 2+ players
- [ ] Uses UUID for player identification
- [ ] Handles disconnections (players leaving)
- [ ] Mobile-friendly design
- [ ] No external dependencies (host assets locally)
- [ ] README.md with rules and controls
- [ ] Tested on actual hardware (ESP32 + SD card)
- [ ] No hardcoded IPs (use `window.location.hostname`)
- [ ] Clean console (no errors or warnings)

### Game Folder Structure

```
MiniSD/games/your_game/
├── index.html       ← Entry point (required)
├── README.md        ← Documentation (required)
├── game.js          ← Logic (optional, can be inline)
├── style.css        ← Styling (optional, can be inline)
└── assets/          ← Images, sounds, etc. (optional)
    ├── icon.png
    └── sound.mp3
```

### WebSocket Message Format

**Standard message structure:**
```javascript
{
  type: 'action_name',      // Required: message type
  uuid: 'player-uuid',      // Required: player identifier
  timestamp: 1234567890,    // Recommended: for ordering
  data: { ... }             // Optional: game-specific data
}
```

**Example:**
```javascript
// Player makes move
ws.send(JSON.stringify({
  type: 'move',
  uuid: playerUUID,
  timestamp: Date.now(),
  data: {
    position: { x: 100, y: 200 },
    action: 'attack'
  }
}));
```

---

## 🔍 Code Review Process

### What We Look For

**Functionality:**
- Does it work as described?
- Are edge cases handled?
- No memory leaks or crashes?

**Code Quality:**
- Follows style guidelines?
- Well-commented and readable?
- No unnecessary complexity?

**Testing:**
- Includes tests or test instructions?
- Verified on hardware?
- No console errors?

**Documentation:**
- README updated if needed?
- New features documented?
- Breaking changes noted?

### Review Timeline

- **Small PRs** (< 100 lines): 1-2 days
- **Medium PRs** (100-500 lines): 3-5 days
- **Large PRs** (> 500 lines): 1 week+

**Tip:** Smaller PRs get reviewed faster!

---

## 🏆 Recognition

### Contributors

All contributors are listed in:
- GitHub Contributors page
- README.md acknowledgments section
- Release notes for significant contributions

### Game Creators

Games include author attribution:
```html
<!-- In game HTML -->
<meta name="author" content="Your Name">
<footer>Created by Your Name</footer>
```

---

## 📞 Getting Help

### Questions?

- **General questions:** Open a GitHub Discussion
- **Bug reports:** Create an issue with bug template
- **Feature ideas:** Create an issue with feature template
- **Security issues:** Email [security email] (do NOT open public issue)

### Community

- **GitHub Discussions:** Best for Q&A and ideas
- **Issues:** For bugs and feature requests only
- **Pull Requests:** For code contributions

---

## ⚖️ License

By contributing, you agree that your contributions will be licensed under the MIT License (see [LICENSE](../LICENSE)).

---

## 🙏 Thank You!

Every contribution, no matter how small, makes this project better. Thank you for being part of the LAN Party Arcade community! 🎮

---

**Last Updated:** February 2026  
**Questions?** Open a GitHub Discussion or issue.

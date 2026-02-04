# ESP32 Multiplayer Game Platform

## Project Overview

A modular, open-source platform that transforms the ESP32-2432S028 ("Cheap Yellow Display") into a self-contained multiplayer game server. Players connect via their mobile phones to a local WiFi network hosted by the ESP32. Games are stored on hot-swappable SD cards, acting as physical "game cartridges" - inspired by the classic Nintendo cartridge model.

## Core Concept - The Nintendo Model

**ESP32 = NES Console**
- Boots the game cartridge
- Creates WiFi network for controllers
- Routes messages between players
- Serves game files
- **Does NOT run game logic** - just relays messages

**SD Card = Game Cartridge**
- Self-contained game in a folder
- Plug and play - swap cards to change games
- All game logic runs client-side on phones
- ESP32 just serves files

**Mobile Phones = Personal Displays & Controllers**
- Players access games through phone browsers (no app installation)
- Each phone shows private information unique to that player (your cards, your choices, your score)
- Interactive controls for game actions (bet, fold, vote, draw, etc.)
- All game logic runs on the phones (faster than ESP32)
- One phone can be designated as "game display" for shared view

**Physical Display = System Info Screen**
- Shows connection instructions (WiFi name, password, QR code)
- Lists connected players
- Shows current game loaded
- System diagnostics (uptime, memory, player count)
- **Does NOT render game graphics** (V2.0 feature)

## Hardware Platform

**ESP32-2432S028 Features:**
- 2.8" TFT LCD touchscreen (320x240, ILI9341)
- Resistive touch controller (XPT2046) - *V2.0 feature*
- ESP32 dual-core processor
- WiFi and Bluetooth
- SD card slot
- 4MB flash (typical)
- RGB LED, LDR sensor
- Exposed GPIO pins

**Cost:** ~$10-15 USD

## Technical Architecture

### ESP32 Responsibilities (Keep It Simple!)

The ESP32 is intentionally "dumb" to maximize reliability and minimize development complexity:

1. **WiFi Access Point** - Create local network with auto-collision handling
2. **DNS Server** - Wildcard DNS (any domain → ESP32 IP)
3. **File Server** - Serve HTML/JS/CSS from SD card
4. **WebSocket Relay** - Pass messages between connected clients
5. **Connection Registry** - Track who's currently connected (UUID → socket mapping)
6. **Display Manager** - Show system info on physical screen

**That's it.** No game logic, no state management, no rules enforcement.

### Client-Side Architecture (Where the Magic Happens)

**Game logic runs entirely in the browser:**
- State management via Zustand
- Game rules and validation
- Turn management
- Victory conditions
- Animations and rendering

**Host-Based State:**
- First player to join becomes "host"
- Host's game state is source of truth
- Host broadcasts state updates via WebSocket
- ESP32 relays host's updates to all other players
- Automatic host migration if host disconnects

### Network & Communication

**WiFi Access Point:**
- Default SSID: `LAN_Party_Arcade`
- Auto-appends number if collision detected (`LAN_Party_Arcade_2`, `LAN_Party_Arcade_3`, etc.)
- Open network by default (no password)
- Configurable via `config.json` on SD card
- DNS wildcard: ANY domain resolves to ESP32 (type `play`, `game`, or anything → works!)

**Connection Flow for Players:**
1. Scan WiFi QR code on display (auto-joins network)
2. Open browser
3. Type `play` (or any word - DNS wildcard handles it)
4. Automatically loads game

**WebSocket Message Flow:**
```
Player 1 (Host) → ESP32 → Player 2, Player 3, Display
Player 2 Action → ESP32 → All Clients (including host)
```

ESP32 is a pure relay - doesn't interpret messages, just routes them.

**State Synchronization:**
- Full state broadcasts (not deltas)
- Zustand on clients handles efficient re-rendering
- Framework provides two update patterns:
  - `useGameState()` - Standard for turn-based games
  - `useContinuousState()` - Optimized for real-time/timers

**Performance:**
- Supports 10-20 simultaneous connections
- Can handle 50-100 updates/second across all clients
- Good for timers, animations, real-time games

### Player Roles

**Player Controllers:**
- Each player's phone is their personal display and control panel
- Shows private information only they can see (your hand in poker, your role in Werewolf, your drawing in Pictionary)
- Unique UI tailored to their current state (different buttons/options based on game phase)
- Touch-optimized controls for game actions
- Send actions to other players via ESP32
- **Think:** Your phone is your "player board" in a board game - personal and private

**Game Display:**
- Dedicated phone/tablet positioned in center
- Same React app with `?role=display` parameter
- Shows shared information (community cards, scores, board state)
- Larger, better screen than ESP32's 2.8" display
- Touch interaction available (same React app)

**Host:**
- First player to connect
- Maintains authoritative game state
- Auto-migrates to next player if disconnected

## SD Card Structure

```
/sdcard/
  ├── _framework/                # Shared React framework (future)
  │   ├── hooks/
  │   │   ├── useGameState.js
  │   │   ├── useContinuousState.js
  │   │   ├── useWebSocket.js
  │   │   ├── useHostMigration.js
  │   │   └── usePlayers.js
  │   ├── components/
  │   │   ├── Lobby.jsx
  │   │   ├── PlayerList.jsx
  │   │   └── ConnectionStatus.jsx
  │   └── utils/
  │       └── websocket-manager.js
  │
  ├── poker/
  │   ├── manifest.json          # Game metadata
  │   ├── index.html             # Entry point
  │   ├── game-logic.js          # Client-side game rules
  │   ├── components/
  │   │   ├── PlayerView.jsx
  │   │   └── DisplayView.jsx
  │   └── assets/
  │       ├── images/
  │       └── sounds/
  │
  ├── codenames/
  └── trivia/
```

### Manifest.json Format

```json
{
  "name": "Texas Hold'em",
  "version": "1.0.0",
  "minPlayers": 2,
  "maxPlayers": 8,
  "entry": "index.html",
  "description": "Classic poker game",
  "author": "Community Contributor",
  "reconnectGracePeriod": 300,
  "pauseOnDisconnect": true
}
```

## Session Management & Reconnection

### Player Sessions

**On First Connect:**
- Browser generates UUID (stored in `sessionStorage`)
- Player claims a seat/slot (Player 1, Player 2, etc.)
- UUID associated with that slot

**On Disconnect:**
- Grace period: **5 minutes default** (configurable per game)
- Game pauses by default (developer can override)
- Other players see "Player 2 disconnected..."
- Slot remains reserved

**On Reconnect:**
- Browser sends stored UUID
- UUID match takes priority - reclaims original slot
- If someone else joined during disconnect, new player becomes next available slot
- Seamless rejoin with same state

**Browser Sleep Prevention:**
- Wake Lock API keeps screen active during player's turn
- Heartbeat pings every 30 seconds to maintain connection when backgrounded
- Framework handles this automatically

## Framework Philosophy

### Guidelines, Not Limits

- **Recommendations provided**, not strict requirements
- State size guideline: <10KB for best performance
- But if your game needs 50KB? Go for it - just test it
- Framework provides debugging tools (state size monitor, performance warnings)
- Developers can push boundaries and experiment

### Toolkit Approach

**Framework provides building blocks:**
- Pre-built hooks for common patterns
- Example games showing best practices
- Components you can use or ignore
- Multiple approaches to same problems

**Developers choose:**
- Which hooks to use
- Update patterns (standard vs continuous)
- State structure
- How much framework to leverage

**80% of games** will use core patterns out of the box.
**20% of games** will customize heavily - and that's encouraged!

## Development Workflow

### For Game Developers

**You write:**
1. `manifest.json` - Game config
2. `game-logic.js` - Client-side rules and state
3. `PlayerView.jsx` - What each player sees on their phone
4. `DisplayView.jsx` - Shared screen in the middle of table
5. `assets/` - Images, sounds, etc.

**Framework provides:**
- WebSocket connection handling
- State management (Zustand setup)
- Session management and reconnection
- Lobby/waiting room
- All the hooks and utilities

**Testing:**
- Develop locally with any web server
- Test with multiple browser tabs
- Deploy = copy folder to SD card

### Starter Templates

**Three complexity levels:**

1. **Dead Simple** - Dice Roller
   - Minimal state, no turns
   - Shows basic WebSocket usage
   - ~50 lines of code

2. **Turn-Based** - Tic-Tac-Toe
   - Turn management
   - Win condition checking
   - Host migration example

3. **Complex** - Simple Poker
   - Private state (player hands)
   - Public state (community cards)
   - Betting rounds
   - Multiple phases

## ESP32 Firmware Architecture

### Boot Sequence

1. Initialize hardware (display, SD card, WiFi)
2. Read `config.json` from SD card (if exists)
3. Create WiFi AP with collision handling
4. Start DNS server (wildcard mode)
5. Read game manifests from SD card
6. Serve `index.html` (game selection handled by developer)
7. Display connection instructions (WiFi QR code + "type play")

### Boot Configuration

**config.json** (optional, on SD card root):
```json
{
  "wifiSSID": "LAN_Party_Arcade",
  "wifiPassword": "",
  "maxConnections": 20
}
```

- Empty password = open network
- SSID auto-appends numbers on collision
- If config missing: defaults to `LAN_Party_Arcade_[MAC last 4]`
- Display shows actual SSID created (not config value)

### Runtime Behavior

**Display Shows:**
- WiFi QR code (scan to auto-join network)
- WiFi SSID (actual name after collision handling)
- "Type: play" instructions
- Connected players (names/count)
- Current game name
- System stats (uptime, memory) - toggle via touch

**WebSocket Server:**
- Accept connections
- Assign UUIDs to new clients
- Relay all messages between clients
- Track active connections
- Clean up on disconnect

### Error Handling

**Philosophy: Fail gracefully, show helpful info on display**

**SD Card Issues:**
- No card detected → Display shows "Insert SD card"
- No games found → Display shows "No valid games on card"
- Corrupted files → Skip that game, load next valid one

**Connection Issues:**
- Max connections reached → New players see "Game full" in browser
- WebSocket overflow → Drop oldest idle connection (with warning)
- Malformed messages → Log to serial, ignore message, don't crash

**Keep it simple:** ESP32 doesn't try to "fix" problems, just communicates them clearly.

### Display Layout Design

**2.8" screen at 320x240 requires ruthless prioritization.**

**Screen 1 (default):**
```
┌─────────────────────┐
│  JOIN THE GAME      │
│                     │
│  [WiFi QR Code]     │
│                     │
│  LAN_Party_Arcade   │
│  (scan to connect)  │
│                     │
│  Then type: play    │
│                     │
│  Players: 3         │
└─────────────────────┘
```

**Screen 2 (tap to switch - V1.0 stretch goal):**
```
┌─────────────────────┐
│   SYSTEM INFO       │
│                     │
│   Uptime: 02:34:15  │
│   Memory: 234KB     │
│   Players: 3/20     │
│   Game: Poker       │
│                     │
│   IP: 192.168.4.1   │
└─────────────────────┘
```

**Touch interaction:**
- Single tap switches screens
- Simple toggle between connection info and system stats
- Prepares for V2.0 touch features

### Game Selection & Menu

**Developer's responsibility** - ESP32 serves `index.html`, developer builds the lobby.

**Standard pattern (framework provides):**
- `<GameSelector>` component (optional)
- Shows available games from manifest files
- Built-in voting system: majority wins, host breaks ties
- Developers can customize or skip entirely

**ESP32 behavior:**
- Serves `index.html` for ANY URL request
- Optional: Provides `/api/games` endpoint listing available manifests
- Client handles all selection logic

**That's the entire firmware.** ~500-1000 lines of C++ total.

## Example Use Cases

### Party Games
- Trivia competitions
- Drawing games (Pictionary-style)
- Voting/poll games
- Word association
- Charades prompts

### Board Game Companions
- Codenames coordinator
- Secret Hitler/hidden role games
- Werewolf/Mafia moderator
- Dice roller with history

### Card Games
- Texas Hold'em
- Blackjack
- Poker variants
- Custom card games

### Cooperative Games
- Escape room puzzles
- Team challenges
- Mystery solving

## Version Roadmap

### V1.0 - Foundation (Current Focus)

**Goals:**
- ESP32 firmware prototype
- Client-side framework basics
- 2-3 example games
- Documentation for developers

**Scope:**
- System info display only (no game rendering)
- No touch interaction on ESP32
- Single game auto-loads (no menu selection)
- Client-side everything
- No cheat prevention

### V2.0 - Enhancements (Future)

**Potential Features:**
- Touch interaction on ESP32 display
- Game selection menu on physical screen
- LVGL game rendering on display (optional)
- Anti-cheat validation modes
- Save/resume game state
- Sound effects
- Bluetooth controller support

**Philosophy:** Get V1.0 solid first, then expand based on real usage.

## Community & Distribution

### Project Structure

**GitHub Organization:**
- Main repo: ESP32 firmware + framework
- Game repos: One per game (easy forking)
- Template repo: Starter kit
- Docs repo: Tutorials and guides

### Game Packaging

**Simple approach:**
- Games are folders on SD card
- Copy folder = install game
- Share folder via GitHub, zip files, etc.

**Future:** `.epg` format (ESP32 Party Game)
- Standardized zip with manifest
- One-click install via web tool
- Community game browser

### Contribution Areas

- New game implementations
- Framework improvements
- Example games and tutorials
- Documentation
- Bug reports and testing
- UI components and assets

## Design Principles

✅ **Accessible** - No coding to play, minimal coding to create
✅ **Simple** - ESP32 does less, clients do more
✅ **Modular** - Cartridge model, swap and play
✅ **Open** - Community-driven, MIT licensed
✅ **Portable** - Self-contained, no internet required
✅ **Affordable** - ~$10-15 hardware cost
✅ **Flexible** - Guidelines not limits, toolkit not framework
✅ **Fast** - Client-side execution, no server bottleneck

## Technical Constraints & Considerations

### ESP32 Limits
- ~520KB usable RAM (with WiFi active)
- SD card read speed: ~100-200KB/s
- WebSocket connections: 10-20 comfortable max

### Mitigations
- GZIP compression for served files
- Client-side caching
- Asset size warnings in manifest
- Framework provides performance monitoring

### Security
- Local network only (no internet exposure)
- Game validation is social (playing with friends)
- Cheating considered "feature" for house rules
- V2.0 can add "verified mode" if needed

## Getting Started

### For Players
1. Power on ESP32
2. Scan WiFi QR code on display (or manually join `LAN_Party_Arcade`)
3. Open browser, type `play`
4. Select game and join
5. Play!

### For Developers
1. Clone starter template
2. Write game logic in `game-logic.js`
3. Design player view in `PlayerView.jsx`
4. Design display view in `DisplayView.jsx`
5. Test locally
6. Copy to SD card
7. Share with community!

## ESP32 Development Plan - Phased Approach

**Philosophy: Baby steps, test each phase before moving on.**

### Phase 1: Basic Hardware & Display
**Goal:** Prove the hardware works
- Initialize ESP32
- Get display working (show "Hello World")
- Test SD card mounting/reading
- **Success criteria:** Text appears on screen, SD card detected

### Phase 2: WiFi Access Point
**Goal:** Create network, handle collisions
- Create WiFi AP with default SSID
- Test SSID collision detection/auto-append
- Read config.json for custom SSID
- Display actual SSID on screen
- **Success criteria:** Phone can see and join the network

### Phase 3: DNS Server
**Goal:** Make "play" work in browser
- Add DNS server (wildcard mode)
- Test: type "play" → resolves to ESP32
- Test: type anything → works
- **Success criteria:** Any word in browser reaches ESP32

### Phase 4: File Server
**Goal:** Serve basic HTML from SD card
- Serve static files from SD root
- Test with simple index.html
- Handle 404s (redirect to index)
- **Success criteria:** Browser shows served HTML page

### Phase 5: WebSocket Relay
**Goal:** Pass messages between clients
- WebSocket server setup
- Accept connections, assign UUIDs
- Echo messages back (simple test)
- Broadcast to all connected clients
- **Success criteria:** Two browsers can send messages through ESP32

### Phase 6: Connection Registry & Display Updates
**Goal:** Track players, show on screen
- Track connected clients (UUID → socket)
- Update display with player count
- Handle disconnects gracefully
- **Success criteria:** Display shows "Players: 3" in real-time

### Phase 7: Polish & Error Handling
**Goal:** Make it robust
- WiFi QR code generation on display
- Error messages (no SD card, etc.)
- Touch toggle between screens (stretch goal)
- Memory monitoring
- **Success criteria:** Handles edge cases gracefully

**Each phase: test, validate, commit before moving on.**

---

## Summary

This platform combines the simplicity of the Nintendo cartridge model with modern web development to create a unique multiplayer gaming experience. By keeping the ESP32 "dumb" and pushing logic to clients, we maximize performance, simplify development, and create a truly modular system.

The physical SD card "cartridge" provides nostalgic appeal while React and WebSockets offer familiar, powerful development tools. Together, they create a platform that's both fun to use and fun to develop for.

**Let's build games together!** 🎮

---

*Version 2.0 - Updated after design discussions*
*Focus: Client-side heavy, Nintendo cartridge model, guidelines over limits*

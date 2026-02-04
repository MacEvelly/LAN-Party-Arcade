# Dice Roller - Example Game Template

This is a minimal example showing how to build a multiplayer game for the LAN Party Arcade platform.

## Features Demonstrated

- ✅ WebSocket connection to ESP32 (port 81)
- ✅ UUID-based player identification
- ✅ Broadcasting actions to all connected players
- ✅ Receiving and displaying other players' actions
- ✅ Auto-reconnect on disconnect
- ✅ Session persistence (UUID saved in sessionStorage)

## File Structure

```
_example_dice_roller/
├── index.html          # Complete single-file game
└── README.md           # This file
```

## How It Works

### 1. **Connection**
- Connects to WebSocket server at `ws://<hostname>:81`
- Generates or retrieves UUID from sessionStorage
- Sends `init` message with player UUID

### 2. **Game Logic**
- Player clicks "Roll Dice"
- Random number (1-6) generated client-side
- Result broadcasted to all players via WebSocket

### 3. **Message Format**
```javascript
// Dice roll message
{
  type: 'dice_roll',
  uuid: 'player-uuid-here',
  result: 4,
  timestamp: 1234567890
}
```

### 4. **Relay Behavior**
- ESP32 receives message from Player A
- ESP32 broadcasts to all players EXCEPT Player A
- Other players see "Player rolled 4 ⚃"

## Using as Template

### To Create a New Game:

1. **Copy this folder**
   ```
   MiniSD/games/_example_dice_roller/ 
   → MiniSD/games/your_game_name/
   ```

2. **Modify the game logic**
   - Change `rollDice()` to your game action
   - Update message types (`dice_roll` → `your_action`)
   - Add your game UI elements

3. **Keep WebSocket structure**
   - Keep UUID generation
   - Keep connection/reconnection logic
   - Keep message format (type, uuid, timestamp)

4. **Test with multiple devices**
   - Open on phone 1: `http://play.local/games/your_game_name/`
   - Open on phone 2: Same URL
   - Actions should sync in real-time

## Key Concepts

### **Client-Heavy Architecture**
- ESP32 is just a "dumb relay" - no game logic
- All game rules run on the phone
- ESP32 only forwards messages between players

### **Why This Works**
- Fast: No server processing delay
- Scalable: ESP32 doesn't do any computation
- Flexible: Change games without reprogramming ESP32

## Customization Ideas

- **Card Games**: Broadcast card draws, plays
- **Trivia**: Share question advances, answer selections
- **Party Games**: Voting, responses, timers
- **Turn-Based**: Send turn state, player actions

## Resources

- Full documentation: `/docs/GAME_DEVELOPMENT.md` (future)
- WebSocket test page: `http://play.local/test.html`
- Message debugging: Browser console (F12)

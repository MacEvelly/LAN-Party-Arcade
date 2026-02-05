---
name: Game Submission
about: Submit a new game for the arcade
title: "[GAME] "
labels: game, enhancement
assignees: ''
---

## Game Information
- **Game Name**: 
- **Genre**: (e.g., Party, Puzzle, Action, Trivia)
- **Players**: (e.g., 2-6 players)
- **Playtime**: (e.g., 5-15 minutes per round)

## Description
Brief description of the game mechanics and objective.

## Features
- [ ] Works with 2+ players
- [ ] Uses UUID for player identification
- [ ] Handles player disconnections gracefully
- [ ] Mobile-friendly (responsive design)
- [ ] No external dependencies
- [ ] Includes README.md with instructions
- [ ] Tested on actual ESP32-2432S028 hardware
- [ ] No hardcoded IP addresses

## Files
List the files included in your game submission:
- `index.html`
- `style.css`
- `game.js`
- `README.md`
- (any other assets)

## WebSocket Messages
Document the message types your game uses:
```json
{
  "type": "game_action",
  "uuid": "player-uuid",
  "timestamp": 1234567890,
  "data": {
    // your game-specific data
  }
}
```

## Screenshots
Include screenshots of:
1. Game lobby/start screen
2. Active gameplay
3. Mobile view (if different)

## Testing Checklist
- [ ] Tested on Chrome desktop
- [ ] Tested on Chrome mobile (Android/iOS)
- [ ] Tested with 2 players
- [ ] Tested with maximum players
- [ ] Tested player disconnect/reconnect
- [ ] No console errors
- [ ] Responsive on different screen sizes

## Additional Notes
Any special considerations, known limitations, or future improvements?

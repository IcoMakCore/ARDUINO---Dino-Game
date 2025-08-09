# Arduino Dino LCD Game

A simple **Dino Run**-style game playable on a **16×2 LCD** with Arduino with a leaderboard controlled by processing.  
The player controls a dinosaur that must jump over obstacles (cacti) using a single button.  
Includes background music, live score display, and collision detection.

## Features
- Dinosaur and obstacle animation on **16×2 LCD** (HD44780 or compatible)
- Single-button control (jump)
- Background game music via buzzer
- Player name input via serial from the PC
- Incremental score up to **999** (victory condition)
- Automatic restart after game over

## Hardware Components

| Component                  | Qty | Notes                                         |
|----------------------------|-----|-----------------------------------------------|
| Arduino UNO/Nano/Mega      | 1   | Any board compatible with LiquidCrystal       |
| 16×2 LCD HD44780 (4-bit)   | 1   | Wired to pins 4,5,6,7,8,9                      |
| Push button                | 1   | One side to GND, other to pin 3                |
| Active/Passive buzzer      | 1   | Connected to pin 2                            |
| Jumper wires               | —   | Male–Male / Male–Female as needed              |
| Breadboard                 | opt | For easier prototyping                        |
| USB cable                  | 1   | For power and serial communication            |

## Wiring

| LCD Pin | Arduino Pin |
|---------|-------------|
| RS      | 9           |
| E       | 8           |
| D4      | 4           |
| D5      | 5           |
| D6      | 6           |
| D7      | 7           |

**Other connections:**
- Push button → Pin 3 (`INPUT_PULLUP`) and GND
- Buzzer → Pin 2 and GND
- VCC and GND for LCD and buzzer power

## How to Play
1. Wire the hardware according to the table above.
2. Upload the sketch to your Arduino.
3. Open the Serial Monitor and set **9600 baud**.
4. Enter your player name and press **Enter**.
5. Press the button to start.
6. Avoid the cacti by timing your jumps correctly!
7. If you reach **999 points**, you win.

## Resetting the High Score
- Hold the jump button for **3 seconds** during gameplay.

## Software Requirements
- Arduino IDE 1.8+ or Arduino CLI
- Libraries:
  - `LiquidCrystal` (default)
  - `EEPROM` (default)
- Processing
## License
MIT License – feel free to modify and share.

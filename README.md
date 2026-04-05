# Game of Life

Conway's Game of Life - a cellular automaton where life emerges from simple rules.

## Configuration

Edit `configs/setup.json` to set field size and step delay.  
If the file is missing, defaults apply: **field size 30**, **step delay 0.05 s**.

## Keybinds

| Key | Action |
|-----|--------|
| `SPACE` | Pause / resume |
| `C` | Clear field |
| `S` | Save field |
| `P` | Paste field |

## File slots

Hold a modifier key and press a digit `0–9`:

| Combo | Action |
|-------|--------|
| `W` + digit | Write field to slot |
| `R` + digit | Read field from slot |
| `D` + digit | Delete slot |

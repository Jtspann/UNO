# UNO

A command-line implementation of the UNO card game written in C++, supporting 2–10 players on a single machine.

## Features

- 2–10 players with custom names
- Full standard UNO deck (numbered cards 0–9, Skip, Reverse, +2, Wild, Wild +4)
- Turn-based gameplay with clockwise/counter-clockwise direction
- UNO call system — type `u` after your card selection when you have 2 cards
- Call out players who miss their UNO call with `c`
- Draw a card by entering `-`

## How to Play

1. Run the executable
2. Enter player names one at a time, pressing Enter after each; type `-` when done
3. On your turn, you'll see the top card and your hand displayed by color, value, and index
4. Enter the index number of the card you want to play
   - Append `u` to call UNO (e.g. `1u`)
   - Append `c` to call out a player who missed UNO (e.g. `4c`)
   - Enter `-` to draw a card
5. First player to empty their hand wins

## Building

Requires a C++ compiler with C++14 support and Windows (uses `Sleep` and `system("CLS")`).

**Visual Studio:**
Open the `.cpp` file in Visual Studio and build with Ctrl+Shift+B.

**Command line (MSVC):**
```
cl /std:c++14 "ECGR 2103 - 1-1.cpp"
```

**MinGW/g++:**
```
g++ -std=c++14 -o uno "ECGR 2103 - 1-1.cpp"
```

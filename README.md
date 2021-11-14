# teensy_mcp23017_interrupts
Teensy 4.1 + multiple CQRobot i2c MCP23017, examples for input from buttons and output to LED, uses interrupts.

Uses the Adafruit MCP23017 library, but not the latest version (2.x). I used version 1.2. The 2.x version would not compile for Teensy 4.1.

Using interrupts is much faster than polling every loop.

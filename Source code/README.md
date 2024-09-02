This project is designed for the ATmega328P (8-bit) microcontroller and is built without relying on Arduino libraries. It uses the AVR GCC 8-bit compiler and the standard C++ libraries. The modified TM1637Display library included in this repository is essential for the display functionality.

# Microcontroller
ATmega328P (8-bit)

# Modifications
Converted from Arduino to AVR: The code has been fully adapted to work with AVR libraries, removing dependencies on Arduino libraries.
Removed Unnecessary Functions: Functions related to single-phase AC motors have been removed as the project focuses solely on 3-phase AC motor control.
Added Reverse_3_Phase() Function: This new function allows reversing the rotation direction of the 3-phase AC motor.
Converted Display Libraries: The display functionality has been converted from Arduino-specific libraries to AVR-compatible libraries.

# Instructions for Compilation
To compile this code using the AVR GCC 8-bit compiler, follow these steps:

# 1. Include the Modified Display Library
Ensure you have included the modified TM1637Display library provided in this repository. This library is crucial for the correct operation of the displays used in this project.

# 2. Setting Up the Environment
Verify that the AVR GCC 8-bit compiler is correctly installed on your system.

# 3. Compiling the Code
Use your preferred IDE or command-line tools compatible with AVR GCC to compile the code. Ensure the modified TM1637Display library is included in the compilation process.

# Important Notes
Arduino-Free: This project does not use Arduino libraries and relies solely on AVR libraries and standard C++ libraries.
Port Requirements: The display functionality is designed to work exclusively with Port B pins. The code will not function correctly if other ports are used.

# License
This project is licensed under the MIT License.

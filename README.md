# Variable Frequency Drive (VFD) for AC Motors

## Introduction
This project involves the design and development of a Variable Frequency Drive (VFD) for controlling the speed and torque of AC motors. The VFD adjusts the motor's input frequency and voltage, enabling precise control over motor performance in various industrial and commercial applications. This project is part of the EN2160 - Electronic Design Realization module at the University of Moratuwa.
<img src="https://github.com/LahiruCooray/VariableFrequencyDrive_Project/raw/37cd681a76083389ce04b7a403ac014a454efe62/Photos%20and%20Videos/Prototype%20-%20VFD.jpg" alt="Prototype VFD" width="600"/>

## Electronic Design
### Power Circuit
The Power Circuit is responsible for converting the high-voltage AC input to a stable DC output, which powers the motor and other components. Key elements include a full-wave rectifier, smoothing capacitors, and AC-DC converters. The design also incorporates a bypass relay circuit to manage inrush currents.

### Intelligent Power Module (IPM)
The IPM is the heart of the VFD, combining high-speed switching IGBTs with integrated protection features like overcurrent and thermal shutdown. It interfaces with the microcontroller unit (MCU) to receive control signals and provide system feedback.

### Microcontroller Unit (MCU)
The MCU controls the VFD operation by generating PWM signals that modulate the output frequency and voltage to the motor. It also manages system diagnostics, user input, and communication with the IPM. The MCU used in this project is the ATmega328-PU.
<img src="https://github.com/LahiruCooray/VariableFrequencyDrive_Project/raw/4da9d129a2faa73fc7f958fe920dd5c11052bff0/Photos%20and%20Videos/System%20Integration%20-%20VFD.jpg" alt="System Integration - VFD" width="600"/>


## Enclosure Design
The enclosure for the VFD is designed to house all the electronic components securely, providing protection against environmental factors and physical damage. The enclosure is made of ABS material, designed with both top and bottom parts that are molded to fit the internal circuits. Detailed design drawings and 3D-printed prototypes were developed to ensure proper fit and functionality.

## Source Code
The source code for this project is written in C++ and is responsible for controlling the MCU. The code handles PWM generation, user inputs via potentiometers and switches, and displays motor parameters on digital LED displays. The code is optimized for real-time motor control, ensuring accurate and responsive operation of the VFD.

## Testing and Prototyping
Extensive testing and prototyping were conducted to ensure the reliability and performance of the VFD. This included PCB testing for power circuits, IPM, and MCU integration, as well as real-time testing with an AC motor to verify speed and torque control. The final prototype demonstrated successful operation under various load conditions, confirming the design's effectiveness.
<img src="https://github.com/LahiruCooray/VariableFrequencyDrive_Project/raw/df00b9ecf799b6f5d75307045a210b49fa528abd/Photos%20and%20Videos/PWM%20Generation.jpg" alt="PWM Generation" width="600"/>





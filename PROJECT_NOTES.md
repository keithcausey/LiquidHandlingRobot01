# GRBL ESP32 Project Context

## Project Overview
- **Project Name**: GRBL ESP32 Liquid Handling/Pipetting Robot
- **Date Started**: October 28, 2025
- **Previous Context**: Migrated from stirringbioreactor project - lost chat context during transition

## Current Objectives
- Testing GRBL ports on ESP32S3 for liquid handling robot
- Configuring 4-axis machine (X,Y,Z motion + A-axis syringe pump)
- Validating 74HC595 shift register control for stepper drivers
- Testing serial communication and port configuration

## Hardware Setup
- **ESP32 Board**: ESP32S3 Development Kit
- **Serial Ports**: COM11 and COM7 (currently using COM7)
- **Axes**: 4-axis system (X,Y,Z motion + A-axis syringe pump)
- **Stepper Control**: 3x 74HC595 shift registers
- **GPIO Assignments**:
  - GPIO 4, 5, 6: Control 74HC595 shift registers
  - GPIO 19: Common stepper enable (ANDed with emergency cutoff)
- **Application**: Liquid handling/pipetting robot

## Testing Status
- [ ] Basic ESP32 communication test
- [ ] Port configuration validation
- [ ] Motor control testing
- [ ] Limit switch functionality
- [ ] Serial communication verification

## Notes
- Need to determine specific machine configuration file being used
- Previous work on stirringbioreactor was using Git successfully
- Lost context during project switch - implementing Git tracking to prevent future loss

## Next Steps
1. Determine current hardware setup
2. Identify appropriate machine configuration
3. Test basic functionality
4. Document any issues or modifications needed

---
*This file tracks the overall project context and progress*
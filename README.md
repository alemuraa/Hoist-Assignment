# Hoist Simulator - Advanced and Robot Programming Assignment

## Overview

This project is part of the Advanced and Robot Programming course and involves the development of a robot simulator. Specifically, it simulates the behavior of a hoist with 2 degrees of freedom (d.o.f.). The simulation is controlled through two graphical user interfaces (GUIs), and several processes are involved in mimicking the hoist's motors and system operations.

## Project Team

- **Miriam Anna Ruggero** (4881702)
- **Alessio Mura** (4861320)
- **Ivan Terrile** (4851947)

## Project Structure

The repository is organized into the following directories:

- **`src/`**: Contains the source code for all processes.
- **`include/`**: Contains data structures and methods used in the ncurses framework to build the GUIs.

After compilation, the following directories will be created:

- **`bin/`**: Contains all the executable files.
- **`log/`**: Contains log files generated during program execution.

## Compilation and Execution

To compile the project, use the following command:

```bash
bash ./compile.sh
```
To run the program, use the following command:
```bash
bash ./run.sh
```
## Simulator Behavior

The hoist simulator operates with the following features:

- **Degrees of Freedom**: The hoist has 2 d.o.f.
- **Command Console**: Allows the user to control the movement of the hoist along two directions.
- **Inspection Console**: Displays the current position of the hoist. It also allows the user to issue commands to:
  - Stop the movement of the hoist.
  - Reset the position of the hoist.

In addition to the main consoles, the simulator includes:

- **Motor Simulation**: Three processes simulate the motors of the hoist.
- **Watchdog Process**: The master process periodically monitors the five other processes. If none of these processes respond for 60 seconds, the watchdog sends a reset command to ensure system stability.

Processes communicate using named pipes and signals, all of which are managed by the master process.

## Notes

- Ensure that you have the required dependencies installed before compiling the project.
- For any issues or bugs, please refer to the log files in the `log/` directory for debugging information.

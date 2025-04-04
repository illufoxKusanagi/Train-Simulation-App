# Train Simulation App

## Overview

The Train Simulation App is a graphical user interface (GUI) application designed to simulate various parameters related to train operations. This application allows users to input and visualize different train parameters, making it a useful tool for educational purposes and engineering simulations.

## Features

- **User -Friendly Interface**: Intuitive layout with easy navigation between different parameter settings.
- **Parameter Input Forms**: Users can input various train parameters such as inertial coefficients, axle numbers, and more.
- **Dynamic Visualization**: The app provides real-time feedback and visualization of the input parameters.
- **Multiple Parameter Categories**: Organizes parameters into categories for better usability.

## Technologies Used

- **Qt Framework**: The application is built using the Qt framework for C++.
- **C++**: The primary programming language used for the application logic.
- **QWidgets**: Utilizes Qt's widget toolkit for creating the GUI.

## Running Project

To run the Train Simulation App, follow these steps:

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/illufoxKusanagi/Train-Simulation-App.git
   ```

2. **Install Qt**: Make sure you have the Qt framework installed on your machine. You can download it from [the official Qt website](https://www.qt.io/download), or click [here](https://d13lb3tujbc8s0.cloudfront.net/onlineinstallers/qt-online-installer-windows-x64-4.9.0.exe) to directly download Qt online installer.

3. **Customize Components** 
   To ensure this project can be run in your device, check these options during Qt installation : 
   [x] Qt Design Studio
   [x] Qt 6.x.x (uncheck the WebAssembly, MSVC, Qt Debug Information Files, Android and Sources options)
   [x] LLVM-MinGW 17.0.6 64-bit
   [x] MinGW 64-bit (latest version)
   [x] MinGW 32-bit (optional)
   [x] CMake 3.30.5
   [x] Ninja 1.12.1

5. **Open the Project**:
   - Open the cloned project in Qt Creator.
   - Open `CMakeLists.txt` file located in project directory.

6. **Build the Project**:
   - Configure the build kit according to your system. Specifically, using MinGW for this project.
   - Use the build option in Qt Creator to compile the application.

7. **Run the Application**:
   - After building, run the application from Qt Creator.

## Installation 

   - Go to [here](https://github.com/illufoxKusanagi/Train-Simulation-App/releases) to find the binaries.
   - Download the latest `TrainSimulationApp.zip` or `TrainSimulationApp.7z` file.
   - Extract the contents to any folder on your computer.
   - Run `TrainSimulationApp.exe` without installation needed.

## Usage

1. Launch the application.
2. Use the left panel to navigate between different parameter categories.
3. Input the desired parameters in the provided forms.
4. Observe the results and visualizations based on the input parameters.

## Contributing

Contributions are welcome! If you would like to contribute to the Train Simulation App, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them.
4. Push your changes to your forked repository.
5. Create a pull request to the main repository.

---

<!-- *This project is maintained by Illufox Kusangi. Released under [License Type].* -->

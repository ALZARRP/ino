# How to Use the VENDOR.ME ESP32 Cheat Engine

This guide will walk you through setting up and using your ESP32 device.

## Prerequisites

1.  **Hardware:** An ESP32-2432S028 CYD (or a compatible ESP32 board).
2.  **Software:** The Arduino IDE installed on your computer.
3.  **USB Cable:** To connect your ESP32 to your computer for flashing.

## Step 1: Setting up the Arduino IDE

Before you can upload the code, you need to configure your Arduino IDE to work with the ESP32.

1.  **Install ESP32 Board Support:**
    *   Open the Arduino IDE.
    *   Go to `File > Preferences`.
    *   In the "Additional Board Manager URLs" field, paste this URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
    *   Click "OK".
    *   Go to `Tools > Board > Boards Manager...`.
    *   Search for "esp32" and install the package by Espressif Systems.

2.  **Install Required Libraries:**
    *   Go to `Tools > Manage Libraries...`.
    *   Search for and install the following libraries one by one:
        *   `LovyanGFX` by lovyan03
        *   `ESPAsyncWebServer` by me-no-dev
        *   `AsyncTCP` by me-no-dev

## Step 2: Compiling and Uploading the Code

1.  **Open the Sketch:**
    *   Open the `esp32_cheat_engine.ino` file in your Arduino IDE.

2.  **Select Your Board:**
    *   Go to `Tools > Board` and find your ESP32 board. A generic "ESP32 Dev Module" usually works well.

3.  **Select the Port:**
    *   Connect your ESP32 to your computer with the USB cable.
    *   Go to `Tools > Port` and select the COM port that your ESP32 is connected to.

4.  **Compile and Upload:**
    *   Click the "Upload" button (the arrow pointing to the right).
    *   The Arduino IDE will compile the code and upload it to your device. This may take a few minutes. You can see the progress in the output console.

## Step 3: Using the Cheat Engine Interface

1.  **Power On the Device:**
    *   After flashing is complete, the ESP32 is ready. You can power it from any USB source, including your computer, a USB wall adapter, or the USB port on your game console (like an Xbox or PlayStation).
    *   The screen on the ESP32 should light up and display **"VENDOR.ME CHEAT ENGINE"**.

2.  **Connect to the WiFi Network:**
    *   On your phone or computer, open your WiFi settings.
    *   Look for a new WiFi network named **"VENDOR.ME Cheat-Engine"**.
    *   Connect to this network. There is no password.

3.  **Access the Web Interface:**
    *   Open a web browser on the device you just connected to the WiFi.
    *   In the address bar, type `192.168.4.1` and press Enter. This is the default IP address for the ESP32's access point.

4.  **Log In and Use:**
    *   You will see the login screen. **You can enter any username and password** and click "Authenticate".
    *   You will then be taken to the main menu page. From there you can navigate to the cheat pages for each game.

## For iOS Users (App-like Experience)

If you are using an iPhone or iPad, you can add the web interface to your Home Screen to make it look and feel like a real app.

1.  Open the web interface in **Safari**.
2.  Tap the "Share" icon (the box with an arrow pointing up).
3.  Scroll down and tap "Add to Home Screen".
4.  Give it a name and tap "Add".

You can now launch the interface directly from your Home Screen.

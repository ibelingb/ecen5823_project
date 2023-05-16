# ECEN 5823 Course Project

The course project aims to build a motion detection system to be installed on the property of a home to either alert the homeowner of movement during specific hours and/or capture images when movement is detected. A PIR motion sensor will be used to detect when motion has occurred and send a signal to a separate BLE connected device to capture an image and/or send an alert to a user. Due to Covid-19, the camera will be simulated by an LED to mimic the action of capturing an image.

The Course Project associated with this repository can be found at the following link.
Course (and Individual) Project Folder: https://drive.google.com/drive/u/1/folders/1mH2x3VJ2vPRzm86isMKA5pesJDOBkiap

# Final Project State and System Functionality
- Motion events able to be detected successfully via PIR sensor.
- Detected motion events triggers the current system time to be logged to flash memory and updated on the LCD screen.
- Motion detection events are only detected within a user specified window (hour increments).
- Data of the last detected motion event time and the current detection schedule are provided to a connected BLE client device.
- A user is able to configure the detection schedule from a BLE client device.
- The current system time, the time of the last motion detection event, and the current detection schedule are displayed on the LCD.
- The detection schedule is written to flash memory on update and read from flash memory on system boot.
- Reading/Writing motion detection information is protected from Man-In-The-Middle (MITM) attacks by utilizing the DisplayYesNo protection scheme.
- The application supports being reprogrammed via Over-The-Air (OTA) updates.

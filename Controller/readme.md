
# Arduino Controller
## Directories
- Controller - Contains all the files required for the Arduino controller.
- ControllerTest - Contains files used for unit testing. Not required for Arduino controller.

## Required Software
The Arduino IDE is required in order to compile this project.
If can be downloaded [here](https://www.arduino.cc/en/software).

### Arduino IDE Libraries
Libraries can be added to the Arduino IDE via **Tools->Manage Libraries...** menu.
The required libraries are:
- NTPClient, by Fabrice Weinberg *(might get removed, as its not currently used)*.

## Arduino Board
This project was developed using an *Arduino Mega with Wifi*, however it should also work with an *Arduino Uno with Wifi*.
The Wifi chip included on the board is a ESP8266.

### Board Configuration
This board configuration info applies to the [Arduino MEGA with Wifi](https://www.jaycar.com.au/mega-with-wi-fi/p/XC4421).

The 8-Pin dip switch settings for the board are:

|                                               | Pin-1  | Pin-2  | Pin-3  | Pin-4  | Pin-5  | Pin-6  | Pin-7  | Pin-8  |
|-----------------------------------------------|--------|--------|--------|--------|--------|--------|--------|--------|
|To program Arduino chip using PC               | *Off*  | *Off*  | **ON** | **ON** | *Off*  | *Off*  | *Off*  | *Off*  |
|To program ESP8266 chip using PC               | *Off*  | *Off*  | *Off*  | *Off*  | **ON** | **ON** | **ON** | *Off*  |
|To communicate between Arduino and PC          | *Off*  | *Off*  | **ON** | **ON** | *Off*  | *Off*  | *Off*  | *Off*  |
|To communicate between ESP8266 and PC          | *Off*  | *Off*  | *Off*  | *Off*  | **ON** | **ON** | *Off*  | *Off*  |
|To communicate between Arduino and ESP8266     | **ON** | **ON** | *Off*  | *Off*  | *Off*  | *Off*  | *Off*  | *Off*  |
|To communicate between Arduino, ESP8266 and PC | **ON** | **ON** | **ON** | **ON** | *Off*  | *Off*  | *Off*  | *Off*  |

When communicating between Arduino, ESP8266 and PC:
- The ESP8266 should be set to use serial3 using the dip switch (set to RXD3-TXD3 instead of RXD0-TXD0)
- The Arduino should be coded to use Serial to communicate with the PC
- The Arduino should be coded to use Serial3 to communicate with the ESP8266

## Compiling Code
There are two separate but related programs within the Controller folder.
Both are compiled from the same source file, **Controller.ino**.

The Arduino IDE must be set to the correct board type before compiling.
The target board can be changed via the **Tools->Boards:...** menu.
The ESP8266 board might not initially be available, but can be added via **Board Manager**.

## Configuring the Compiled Program
The program will need to connect to the local wifi network, and the web server.
This information can be given to the board using the Arduino IDE, via the **Tools->Serial Monitor**.

There are a number of commands which can be used via the serial monitor.
The format is a single letter, optionally followed by some other information.
The command letters can be viewed in the *Controller/interconnect.h* file.

### Set Wifi Login Details
At the time of writing, the command letter to set the wifi login info is 'w' (lower-case).
So to set the wifi login info, type into the serial monitor one of:
- wInsertWifiSSIDNameHere
- wInsertWifiSSIDNameHere+InsertWifiPasswordHere
The 'w' and '+' are markers and must be included. The rest should be updated as required.

The status of the wifi can be checked by using 'W' (upper-case) on its own.

### Set Server Address
At the time of writing, the command letter to set the server address is 's' (lower-case).
So to set the server address, type into the serial monitor an 's', followed by the whole server address (including protocol and optional port).
Should be similar to this: shttp://localhost:5000/api/System

The recorded server address can be retrieved by using an 'S' (upper-case) on its own.

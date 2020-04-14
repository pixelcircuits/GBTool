# GameBoy Tool
The GameBoy Tool is a command line interface for performing basic read and write operations on physical GameBoy, GameBoy Color and GameBoy Advance cartridges. This tool was designed for the Raspberry Pi Zero or Raspberry Pi Zero W (other Raspberry Pi versions not yet supported) with the appropriate hardware connected to the GPIO of the Raspberry Pi. This tool serves as the base functionality for the larger [GBConsole](https://github.com/pixelcircuits/GBConsole) project. Refer to the additional documentation [here](https://github.com/pixelcircuits/GBConsole_Documentation) for information on setting up the hardware.

## Installation
Please follow these steps to install and setup the GameBoy Tool. You will need your Raspberry Pi already setup with [Raspbian Lite](https://www.raspberrypi.org/downloads/raspbian/). Refer to [this tutorial](https://www.raspberrypi.org/documentation/installation/installing-images/README.md) for getting Raspbian up and running (last known working version is 2020-02-13)

### 1. Configure Raspberry Pi
a) From the Raspberry Pi command line run the raspi configuration tool and set the following options
```
sudo raspi-config
	Network Options -> Wi-Fi (setup)
	Interfacing Options -> SPI (enable)
```
b) Reboot the Raspberry Pi and run the following commands to make sure the package manager is up to date
```
sudo apt-get update
sudo apt-get dist-upgrade
```

### 2. Build GameBoy Tool
a) Run the following commands to install dependencies, download the source code and build the GameBoy Tool
```
cd /home/pi
sudo apt-get install -y libssl-dev
git clone https://github.com/pixelcircuits/GBTool.git
cd GBTool
make
```
b) The binary (GBTool) can be found in the GBTool folder once the build process is complete

## Commands
The GameBoy Tool provides the following commands for reading, writing and getting info of connected cartridges

**info** - Gets basic info about the connected cartridge

**readROM -o <output_filename>** - Reads the cartridge ROM and writes the data to the given output file

**readSave -o <output_filename>** - Reads the cartridge save data and writes the data to the given output file

**writeSave -i <input_filename>** - Overwrites the cartridge save data from the given input file

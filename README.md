# portstream

## Installation

```bash
git clone https://github.com/HarveyBates/portstream
cd portstream
mkdir build
cd build
cmake ..
make install
```

## Usage

```bash
portstream -p /dev/tty.usbmodem2101 -b 115200
```

## Example output

```bash
********************************** PortStream **********************************
* Port:                                                   /dev/cu.usbmodem2101 *
* Baud-rate:                                                            115200 *
* Timestamps:                                                          enabled *
********************************************************************************
[21/05 13:40:29.534]: 91
[21/05 13:40:30.534]: 92
```
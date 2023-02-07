# CarSim

CarSim is a small handy tool to simulate some ECUs with UDS diagnostic services. Currently the ECUs don't send any signals and respond only to UDS services `ReadDataByIdentifier` (SID `0x22`) and `DiagnosticSessionControl` (SID `0x10`).

CarSim uses socketcan and can-isotp and is therefore currently only supported on Linux.

## Build instructions

### Using qmake from command line:

```
git clone https://github.com/lindwurm-can/carsim.git
mkdir build
cd build
qmake ../carsim/carsim.pro
make
```

Or use *Qt Creator* to open the project.


##  Start

#### Create virtual socketcan interface
```
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```
#### Start CarSim
```
./carsim vcan0
```

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

# BMS Serial Monitor
The BMS Serial Monitor is a GUI for data from the BMS (Battery Management System).

# Table of Contents
- [BMS Serial Monitor](#bms-serial-monitor)
- [Table of Contents](#table-of-contents)
- [1. Setup Instructions](#1-setup-instructions)
- [2. Usage Instructions](#2-usage-instructions)
- [3. Data Logged](#3-data-logged)
  - [3.1 Raw Data](#31-raw-data)
  - [3.2 Processed Data](#32-processed-data)
- [4. Monitor States](#4-monitor-states)

# 1. Setup Instructions
1. Download and install [Anaconda](https://www.anaconda.com/download)
2. Open terminal
3. Create a conda environment for FEB_BMS_SM (with Python 3.11.5). Press 'y' when prompted.
```
conda create -n FEB_BMS_SM python=3.11.5
```

4. Activate the environment
```
conda activate FEB_BMS_SM
```

4. Install pip packages
```
pip install pyserial serial-tool matplotlib
```

5. Deactivate the environment
```
conda deactivate
```

# 2. Usage Instructions
1. Activate environment
```
conda activate FEB_BMS_SM
```
2. Run "serial_monitor.py" and type responses to questions in terminal
3. When done, deactivate environment
```
conda deactivate
```

# 3. Data Logged
## 3.1 Raw Data
* Battery Voltage (20 x 7)
* Battery Temperature (20 x 7)
* Disabled Temperature Sensors
* BMS State
  * Debug Standard
  * Debug Charge
  * Debug Discharge
  * Debug Shutdown
* Relays
  * Pre-charge Relay (0 = Closed, 1 = Open)
  * Air+ Relay (0 = Closed, 1 = Open)
* Shutdown
  * BMS (0 = Closed, 1 = Open)
  * TSMS (0 = Closed, 1 = Open)
  * IMD (0 = Closed, 1 = Open)
* Charge Data
  * Charger State (0 = On, 1 = Off)
  * Max Charger Current
  * Max Charger Voltage
  * Charger Current
  * Charger Voltage
  * Charger Status Flags (0 = No Error, 1 = Error)
    * Hardware Failure
    * Temperature Protection
    * Input Voltage
    * Starting State
    * Communication State
* Discharge Data
  * Target Voltage
  * Cells Discharging
* IVT Data
  * Voltage U1
  * Voltage U2
  * Voltage U3
  * Current I1

## 3.2 Processed Data
* Voltage: Mean, Min, Max, Std, Range, Total
* Temperature: Mean, Min, Max, Std, Range
* Discharge Data: No. Cells Discharging

# 4. Monitor States
<table>
  <tr>
    <th>State</th>
    <th>Key Press</th>
    <th>About</th>
  </tr>
  <tr>
    <td>Standard</td>
    <td>S</td> 
    <td>-</td>
  </tr>
  <tr>
    <td>Range</td>
    <td>R</td> 
    <td>Highlight out of range values (voltage, temperature)</td>
  </tr>
  <tr>
    <td>Discharge</td>
    <td>D</td> 
    <td>Highlight discharging cells</td>
  </tr>
</table>
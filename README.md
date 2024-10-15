# Movement-Detection-using-24Ghz-Radar
Adaptive movement detection using 24Ghz Radar and CFAR Algorithm


# Radar Movement Detection System
<p align="center">
  <img src="./docs/fig/top.jpg" width="45%" alt="Top View">
    <img src="./docs/fig/bot.jpg" width="45%" alt="Top View">

</p>


## Project Overview
This project implemented a radar-based movement detection system using  IPM165 radar sensor, FreeSoc2 Development Board, along with CA-CFAR algorithm. The system included custom PCB design, radar signal processing, and 3D-printed enclosure.

## Features
- 4-layer PCB design integrating IPM165 radar sensor
- Two-stage bandpass filter with gain of 300
- DMA streaming from ADC to Memory
- Signal processing using Hanning window and FFT
- Movement detection using CA-CFAR algorithm
- Custom 3D-printed enclosure

## Hardware
### PCB Design
- 4-layer PCB designed in Kicad
- Integrates IPM165 radar sensor
- Includes two-stage bandpass filter (Gain = 300)

![PCB](./docs/fig/top_pcb.png)


### Enclosure
- Custom 3D-printed designed in Onshape

![Enclosure](./docs/fig/enclosure.png)

## Software
### Firmware (C)
- DMA streaming from ADC
- Hanning window application
- FFT implementation
- CA-CFAR algorithm for movement detection

### MATLAB
- Verification script for signal processing algorithms


## Results and Performance
![Results](./docs/fig/result.png)


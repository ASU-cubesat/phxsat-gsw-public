**Phoenix Cubesat Ground Station Software v.1.0.**

Released for public use by Phoenix Cubesat Team @ Arizona State University

## About Phoenix Cubesat

As our cities continue to grow, the layout and material composition of urban areas impact what is known as the Urban Heat Island (UHI) Effect: a phenomenon in which the structure of the city causes a rise in surface temperature.

Phoenix is a 3U CubeSat designed by students from Arizona State University which aims to study UHIs from Low Earth Orbit (LEO) through infrared remote sensing. The project is an interdisciplinary collaboration which has joined efforts of 5 major ASU schools to pursue a common goal and make the world a better place for generations to come. 

## Mission objectives

To successfully design, integrate, test, and launch a CubeSat into LEO
Collect infrared images of seven US cities to aid research on the Urban Heat Island Effect
Study the properties which contribute to the Urban Heat Island Effect and work with local communities to help develop a more sustainable urban infrastructure for future generations
Educate the community on the importance of Urban Heat Islands and inspire the next generation to pursue STEM fields


This software contains the core libcsp libraries used to interact with the Phoenix over a radio link. The software package also consists of a Command-Line interface which presently supports reading a heartbeat packet from Phoenix and the ability to direcly send a ping packet to Phoenix. 

In order to communicate with Phoenix you could use a amateur radio tranceiver with UHF capabilities and a TNC that can demodulate the AX.25 packets transmitted using GMSK signal at 9600 baud. The TNC should be in KISS mode.

### Phoenix Transceiver Info:
- Hardware Model: GomSpace AX-100 
- Operating Frequency: 437.35 MHz
- Bandwidth: 20 kHz
- Modulation Type: GMSK (Mod Index: 0.5)
- Baud rate: 9600 bps
- Protocol: AX-25 KISS packets with HDLC encapsulation
- Encoding: NRZI G3RUH/K9NG scrambled

### Ground Station Hardware Requirements:
1. Radio Transceiver
2. TNC
3. Linux Computer
4. UHF Yagi/Antenna
5. Pre-Amplifier (opional)

### Tested Setup:
1. Radio: ICOM IC-9100 with UHF Yagi mounted on a SPID rotor
2. TNC: Kantronics KPC 9612 Plus

### Tracking the satellite:
1. Fetch the TLE for Phoenix from either the phoenix cubesat official website (phxcubesat.asu.edu) or space-track.org (need a space-track account, however)
2. Use a satellite tracking software like GPredict to calculate overhead passes for your location. This could be used with an automatic rotor system that ensures better connectivity. GPredict also helps with dopler shifting on your radio.

### Compile Phoenix Ground Station 
1. Run quickMake.sh to compile and run the ground station software. 

### Sending a ping command via I/Q files (without TNC)
1. There is a recording of the outgoing ping command (I/Q file 32-bit Floating waveform @ 3,200,00 Hz Sample Rate) which can be used in the event a Hardware/Software TNC is not available.
2. The I/Q files file is by the name *PHX_pingto_command.wav* and can be found in the Recordings directory.
  There is an additional file named *PHX_AX100_hk.wav* which is a housekeeping packet from our satellite's UHF radio transceiver.
  A complete breakdown of the HEX translation of each packet alongwith it's packet structure is mentioned in the README.md file contained within the Recording folder.
  Also, there is an audio file named *phxsat-sample-ping-audio.wav*. This could be used directly with applications like SDR# to transmit a ping to the satellite.
3. This can be directly transmitted to the satellite to initiate a ping request.
4. On successful receipt of this request by the satellite you should see a ping back on the same frequency (437.350 MHz NFM).

### Troubleshooting 
- Device not found error.
1. In the source code navigate to main.c
Change the device details (e.g. /dev/ttyUSB0) in line 55


Please refer to http://phxcubesat.asu.edu/ for more details and updates.

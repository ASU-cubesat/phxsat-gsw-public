## **ping 2 - packet**

**NOTE**: The data portion of this packet is the same every time. This command is used for pinging the satellite. The satellite node address is set to 2.

*Data*

    c0 10 96 92 9e 9e 6e b2 60 96 92 9e 9e 6e b2 61 03 f0 
    94 20 70 01 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 
    0e 0f 10 11 12 13 cc 79 eb e6 c0


*Ping 2 Packet Structure*

| data | description |
|--|--|
| c0 10 | start of KISS framing & command byte |
| 96 92 9e 9e 6e b2 60 96 92 9e 9e 6e b2 61 03 f0 | AX.25 header |
| 94 20 70 01 | CSP header |
| 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 | CSP ping command |
| cc 79 eb e6 | CRC32 |
| c0 | end of KISS framing |



## **AX100 hk packet**

**NOTE**: this is not the hk packet that will be transitted from the spacecraft in orbit. We will gather a recording of this post-deployment.
This packet is a housekeeping packet sent from our EM AX100 radio, which contains only the telemetry from the AX100. 

*Data*

    b0 64 94 ae 9e b2 60 96 92 9e 9e 6e b2 61 03 f0 ca a7 c0 00 
    01 18 01 1a 00 00 00 00 00 00 00 0f 00 00 00 1f 00 00 03 66 
    00 00 01 d9 01 40 c5 00 00 00 01 b9 fc bb 0f ff 99 01 f0 0d 
    7d 38 00 8b 59 75 fa 80 c9 90 13 ae 3a 04

	


*AX100 hk packet structure* 
The following data is the telemetry from the AX100 that is captured in the audio file. However, if you are using the ground station
code to decode this, the last 4 bytes (the tot_rx_bytes) will be cut off, as the ground station code normally expects the last 4 bytes to be the CRC of Phoenix's health beacon and cuts these off. 

|data| description |
|--|--|
| b0 64 94 ae 9e b2 60 96 92 9e 9e 6e b2 61 03 f0 | AX.25 header |
| ca a7 c0 00 | CSP header |
| 01 18 | board temp, 280 -> 28C  |
| 01 1a | pa temp, 282 -> 28.2 C |
| 00 00 | last_rssi, 0 |
| 00 00 | last_rferr, 0 |
| 00 00 00 0f | tx_count, 15 |
| 00 00 00 1f | rx_count, 31 |
| 00 00 03 66 | tx_bytes, 870 |
| 00 00 01 d9 | rx_bytes, 473 |
| 01 | active_conf, 1 |
| 40 c5 | boot_count, 16581  |
| 00 00 00 01 | boot_cause, 1 |
| b9 fc bb 0f | last_contact, 3120347919 |
| ff 99 | bgnd_rssi, -103 |
| 01 | tx_duty, 1 |
| f0 0d 7d 38 | tot_tx_count, 4027415864 |
| 00 8b 59 75 | tot_rx_count, 9132405 |
| fa 80 c9 90 | tot_tx_bytes, 4202744208 |
| 13 ae 3a 04 | tot_rx_bytes, 20636580 |



                                                     	 
 

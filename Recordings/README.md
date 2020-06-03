
## **Phoenix Ping Instructions**

Currently Phoenix is operating with only a possible ping command. This command is handled by the onboard transponder (NanoCom AX100 by Gomspace). Although the possibility of this command working depends upon the battery status of the spacecraft.

To send a ping command
1. Start the Command line interface (please refer to the readme file on the repository for more details on this)
2. type the command `ping 5`. Here 5 refers to the node, in this case node 5 represents the AX100. 
3. This should send the packet shown below
   ```
     c0 10 XX XX XX XX XX XX XX XX XX XX XX XX XX XX 03 f0  <<--- AX.25 Header
     14 50 4f 01 01 02 03 04 29 30 8c f4 c0                 <<--- Packet Data 
Here `01 02 03 04` is part of the payload and should be observed in the response packet. 


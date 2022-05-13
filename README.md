# SmartCartPrototypeIoT
Code files for Smart-Cart prototype built using DHT22 &amp; RFID sensors with M5Stack and Raspberry Pi devices

The M5Stack ESP32 Gray device is acts as the node device on the cart, responsible for both:
  1. Reading the current temperature/humidity of the cart ice-box (using the DHT22 sensor), and
  2. Displaying current status of the cart (Items placed + Temperature/Humidity readings of the ice-box)

![Alt text](/media/PXL_20220407_180003969.jpg?raw=true "M5Stack displaying cart status")

The Raspberry Pi device runs an MQTT server to both receive temperature/humidity data from the node and transmit RFID data of items currently placed in the cart (All 3 values are shared via independent topics on the MQTT server).
It also acts as a gateway to the Cloud infrastructure, transmitting values for all 3 parameters.

The arduino file (M5Stack_code_SmartCart.ino) contains code for initialising the M5Stack device and transmitting the DHT22 data. It also reads RFID data from the MQTT server running on the Raspberry Pi, and displays both the contents on its screen as follows:

[Link to video of the cart display prototype](https://github.com/Aditya-Sood/SmartCartPrototypeIoT/blob/main/media/PXL_20220407_180021266.LS_2.mp4)

The JSON file contains the NodeRed responsible for faciliating the read of item's RFID tag and also transmitting the data.

//=================================================================================
// ESP8266-Client-Server
// By: Luis Castro, Cody Krukauskas, Joey Kleiner, Sterling Price
// 07/16/17
//=================================================================================

The aim of this project was to create a web server that could send commands to our
PIC18F4550 microcontroller from the internet. The client connects to the website and then
interacts with the site to send the commands. In this project, we used two protocols:
WiFi and serial commands.

Our PIC18F4550 was programmed with MPLAB X and we used C. The PIC18F4550 would receive
and handle all of the serial commands. Those commands would be sent by the ESP8266
which acted as our web server. The client (anyone who visited the site) would send the
commands by clicking on the site which would then send 'GET' requests to the server.

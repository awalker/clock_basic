A basic 7-segment display clock for ESP8266 and an 8-bit shift register.

Using platformIO.

While the NodeMCU probably has enough pins to wire it directly to the display, I wanted to experiment with the shift register IC. Also this is a test bed for a more complex clock.

# FEATURES
* Configure WiFi connection via captive portal

# TODO
* Configure TimeZone outside of code
* Deal with daylight savings time
* Configure 12/24 mode outside of code
* Seperate shift register logic to own class
  * Use Task instead of delay
* Break NTP task into 3+ tasks for better timing and error handling

# Components
* NodeMCUv1.0 -- an esp8266 board.  AdaFruit Hazzah with probably do.
* 5461AS - 4 digit 7 segment display with decimal points. It is what I had laying around.
* SN74HC595N - 8-bit shift register. 
* 4 resisters - I used 220Ohm. You do you. You need 4 of them.

# WIREING

to be documented.
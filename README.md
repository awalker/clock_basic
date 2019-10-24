A basic 7-segment display clock for ESP8266 and an 8-bit shift register.

Using platformIO.

While the NodeMCU probably has enough pins to wire it directly to the display, I wanted to expeariment with the shift register IC.

# FEATURES
* Configure WiFi connection via captive portal

# TODO
* Configure TimeZone outside of code
* Deal with daylight savings time
* Configure 12/24 mode outside of code

# Components
* NodeMCUv1.0 -- an esp8266 board.  AdaFruit Hazzah with probably do.
* 5461AS - 4 digit 7 segment display with decimal points. It is what I had laying around.
* SN74HC595N - 8-bit shift register. 
* 4 resisters - I used 220Ohm. You do you. You need 4 of them.

# WIREING

to be documented.
Märklin-Motorola Track Protocol Decoder for Arduino
===================================================

Decodes train control messages sent by a Märklin 6021 model train controller (among others).

Setup
-----
You cannot connect an Arduino directly to the track - the signal is alternating ±10V, which would most likely damage an Arduino's 0-5V input pins. Instead, use something like the following:

<img src="http://ss.cpfx.ca/qYKU0SZk.png" width="450">

Tested on an Arduino NG with an Atmega168 - where the track input must be connected on pins 2 or 3, as those are the only pins with interrupt support.

Protocol Info
-------------
See [this comprehensive explanation of the Märklin-Motorola protocol](http://www.drkoenig.de/digital/motoueb.htm).


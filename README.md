
# Introduction
This project is quite involved, so first, why go this route?
There are essentially two ways to control the lights at home, Smart Bulbs or with Smart Light Switches / dimmers, both with advantages and disadvantages. 

Smart Light Switches are cheaper. Purely based on cost, smart light switches are likely to be far cheaper than smart bulbs, especially since one light switch can control multiple light fixtures, depending on how your house’s wiring is set up.

You can use any bulb with Smart Light Switches. Since the traditional light bulb industry is very mature, the range of styles and accessories is much broader.
Smart Light Switches can control your existing fixtures without complex modifications.

The main disadvantage is that special wiring is required in the electric boxes where the switches are to be installed (the wires for hot, neutral, as well as the wire for the light are required). In most recent installations, this is not an issue, but it can be a problem for older installations; also, there are different standards in each country.

While smart bulbs offer more flexibility and have the possibility of adding color, they are more expensive and the need for this level of control, from my point of view, only makes sense in a few specific areas and in the end, you can combine the two solutions.

For these reasons, I leaned towards Smart Light Switches for the bulk of my lights.

Now since there are several options on the market, why go the DIY path?

Mainly I return to the point of the range of styles and accessories. In the arena of traditional light switches, there are thousands of colors, styles, textures and accessory options that are not yet available in smart switches.

On the other hand, inexpensive smart switches do not offer enough functionality, and some of the more expensive ones did not fit the form factor of my installation. 

By taking the DIY path, I can get the flexibility in style, form factor, and functionality at a reasonable cost.

With that out of the way, let’s go to the project itself.

# IoT Double Dimmer

This project consists of an AC light dimmer for one or two incandescent light bulbs or dimmable LED bulbs, with mains AC voltage of 110 to 220 V, 50 or 60Hz, up to 400W, using the WiFi enabled microcontroller ESP-12E / F.

The Dimmer functionality is achieved using the phase control method, using a TRIAC for each bulb, varying the total power applied. This method works very well with incandescent light bulbs and with LED lamps that have the Dimmable specification.

Features:
•	Lights can be dimmed from 10% to 100% power.
•	Supports transitions between brightness levels.
•	One push button per light bulb to control the ON/OFF state and brightness.
•	A LED indicator for each button/light bulb.
•	LED indicators change intensity if light is ON or OFF. 
•	LED indicators intensity can be set remotely or turned off for day / night mode.
•	Smart control and configuration using MQTT over WiFi; An MQTT broker is required.
•	ON/OFF state and brightness can be set via the push buttons or remotely via WiFi/MQTT.
•	Integrated into Home-Assistant or other MQTT compatible IoT Hubs, with auto discovery.
•	Setup Portal over WiFi, to configure WiFi/MQTT parameters.
•	The Setup Portal is started by a double reset or optionally with multi-click of any of the light buttons.
•	On Power Up the device resets to the last known state, using MQTT retain or EEPROM.
•	Optional double click to trigger additional actions.
•	Advanced configuration through MQTT.

For mor details check the documentation at: https://github.com/jamozu/DoubleDimmer/blob/main/RRoble_Double_Dimmer.pdf


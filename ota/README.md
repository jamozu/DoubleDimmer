
# OTA
One of the features in the Dimmer software is to get firmware updates over WiFi using a local HTTP Web server. 
Inside the *"Sketch configuration"* section of the software, see the **OTA_HTTP** definitions. 

This folder includes a test program **update.php** which receives an update request from the Dimmer every time it restarts/reboots.

The test program is developed in PHP, but it can be easily ported to any language supported by your HTTP server.

It is possible to use a HTTPS for a remote server, but this feature is not implemented yet. See [This example]([http://localhost/](https://gist.github.com/igrr/24dd2138e9c8a7daa1b4) for more details.

# update.php

The program receives an update requests from the Dimmers. The Dimmer sends a request on every boot. If the latest version in the server differs from the version installed on the Dimmer, the program sends the binary file with the latest firmware version.

Configure the `$filePath` variable to set the location of the binary firmware files.

# latest.txt

This is a JSON object with the list of hardware options mapped to the firmware files.


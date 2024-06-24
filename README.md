# SomfyController
Control Somfy blinds (with RTS Protocol) througt an Arduino Wemos D1.

It is based from the work of `Nickduino` at [Somfy_Remote](https://github.com/Nickduino/Somfy_Remote)

# Hardware
## Soldering
Solder a 433.42 MHz crystals to replace the one on a 433.92MHz transmitter. A RTL-SDR comes in handy to check the frequency and make sure the transmitter is working.

## Wiring
![Wiring](./doc/wiring.jpg)

# Software
## First step
Once binaries (Core and UI) uploaded. Connect to the Hotspot `SomfyController Fallback Hotspot` (defined in includes/config.h). Use the password `5cKErSRCyQzy` (also defined in includes/config.h). Then, connect to `192.168.4.1` to setup your WiFi connection.

On the UI, you can `create`, `read`, `update` and `delete` remotes.

## API
### REST
You have some endpoints to `control`, `update`, `create` and `delete` remotes. To update WiFi configuration... All documentation about API is defined in the api.html page. (This page is accessible through AP mode or network mode).

### MQTT
TODO

## OTA updates
TODO

# TODO
- [ ] Support non ASCII chars in names ?
- [ ] Add OTA
- [ ] What's happen if a neighbor has the same system ? (change base address ?)
- [ ] MQTT support
- [ ] Improve HTML part
- [ ] Create a HA integration

# Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

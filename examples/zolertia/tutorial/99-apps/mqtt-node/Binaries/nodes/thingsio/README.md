# The things.io packed binaries

## How to flash

Connect a Zolertia device to the USB port, run the following command to see in which USB port has been enumerated:

`./motelist`

Then run:

`./flash-device.sh -d coldchain -p /dev/ttyUSB0`

Replace `coldchain` with the name of the application to be flashed, check the next section for supported applications.


## Available applications

The following applications are available to download and install:

### Cold chain use case

Periodically publish temperature, humidity and light readings, and trigger alarms if any value exceeds a given threshold.
The application also pushes wireless link quality, uptime (in seconds), device's ID and parent address.

The following variables are written:

* temperature
* humidity
* light
* rssi
* ID
* parent
* uptime

The following alarms are created:

* alarm_light
* alarm_temperature
* alarm_humidity

The following commands can be sent:

* leds_toggle
* update_period
* enable_sensor
* reboot
* temperature_thresh
* humidity_thresh
* light_thresh

# heizsystem

ESP8266 Arduino | DS18B20 &amp; MAX6675 powered thermal monitoring of a wood/coal stove.


The DallasTemperature Library and the MAX6675 Library are required to get this compiled and uploaded. You can install them via the Arduino Library Manager.  More info following soon.

## HTTP Endpoints

all temperatures are reported back in celsius, device reboots when one-wire devices glitch/change while running

multiple DS18B20 sensors are reachable via

http://esp8266/ds18b20

```
0x28a64295f0013cae 83.19
0x282e6e95f0013c3c 59.63
0x28533095f0013cd9 47.75
```

an Type K thermocouple is reachable via

http://esp8266/max6675

```
185.33
```

and the device supports remote reboot if the readout sees missing values or weird bahaviour.

http://esp8266/reboot

```
bye bye...
```

see influx.sh for importing the data into an influx/grafana stack.

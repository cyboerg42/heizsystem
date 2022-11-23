#!/bin/bash

HOSTNAME="home"
INFLUX_DB_LOC="http://influxdb:8086/write?db=grafana"
INFLUX_PREFIX="heizsystem"

IFS='
'

while true
do

  raw_data="$(curl --connect-timeout 5 esp8266/ds18b20)"

  for item in $raw_data
  do
    name=$(echo $item | cut -d' ' -f1)
    value=$(echo $item | cut -d' ' -f2)

    curl --connect-timeout 5 -i -XPOST $INFLUX_DB_LOC --data-binary "$INFLUX_PREFIX,host=$HOSTNAME,name=$name value=$value"
  done
 
  raw_data="$(curl --connect-timeout 5 esp8266/max6675)"

  for item in $raw_data
  do
    curl --connect-timeout 5 -i -XPOST $INFLUX_DB_LOC --data-binary "$INFLUX_PREFIX,host=$HOSTNAME,name=$name value=$item"
  done

  sleep 2

done

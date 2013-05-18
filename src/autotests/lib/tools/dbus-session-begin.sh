#!/bin/bash

# start the new dbus session bus and export the env variables it creates
echo "Starting new DBus session..."
eval `dbus-launch --sh-syntax`

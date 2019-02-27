#!/bin/bash

ps aux | grep SmartHome-UP-x86 | grep -v "grep" | awk -F' ' '{print $2}' | xargs kill

./ExeWulianDeviceTest


cd /usr/local/mis/mis-iot-box-service/WulianService


nohup ./SmartHome-UP-x86 --debugmode >> up.log 2>&1 &



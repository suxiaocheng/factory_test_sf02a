#!/bin/bash

echo "Close Mis Iot box service"
ps aux | grep mis-iot-box | grep -v grep | awk -F' ' '{print $2}' | xargs kill

echo "Start NFC Test"
sudo ./ZlgNFCTest 2


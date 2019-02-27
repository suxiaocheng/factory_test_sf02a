#!/bin/bash

df -H | grep "/home/iot/DataStore" | grep -v "grep"
if [ $? -ne 0 ]
then
    echo "*********good********"
else
    echo "have data disk,exit!"
    exit
fi


echo "d

d

d

d

w
" | fdisk /dev/sdb

echo "*******delete ok**********"

echo "n
p
1


w
" | fdisk /dev/sdb && mkfs -t ext4 /dev/sdb1

echo "********make  ok**********"

echo "" >> /etc/fstab
echo "/dev/sdb1               /home/iot/DataStore     ext4    defaults,nofail        0 0"  >> /etc/fstab
echo "" >> /etc/fstab

echo "********fstab ok**********"

mount /dev/sdb1 /home/iot/DataStore

echo "********mount ok**********"


#!/bin/bash

echo -e "Lancement du programme LIDAR MAPPING"
rm -f data_lidar.txt
rm -f position_gtk.txt

echo -e "\nExecution de prog.elf\n"
./prog /dev/ttyUSB0

echo -e "\nExecution de main.py\n"
python3.8 main.py

rm -f data_lidar.txt





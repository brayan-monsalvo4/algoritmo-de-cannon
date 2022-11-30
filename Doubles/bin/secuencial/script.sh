#!/bin/bash

   #2 procesadores

   	echo 0 | sudo tee /sys/devices/system/cpu/cpu2/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu3/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu4/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu5/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu6/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu7/online

   	echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online
   
	   for i in 32 64 128 256 512
	   do
	      #Secuencial
	      ./secuencial 2 $i 100
	   done
	   ./secuencial 2 1024 50
	   ./secuencial 2 2048 20

    #4 procesadores

   	echo 0 | sudo tee /sys/devices/system/cpu/cpu2/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu3/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu6/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu7/online

  	echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu4/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu5/online
   
	   for i in 32 64 128 256 512
	   do
	      #Secuencial
	      ./secuencial 4 $i 100
	   done
	   ./secuencial 4 1024 50
	   ./secuencial 4 2048 20


	#8 procesadores

	echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu2/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu3/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu4/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu5/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu6/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu7/online
	   
	   for i in 32 64 128 256 512
	   do
	      #Secuencial
	      ./secuencial 8 $i 100
	   done
	   ./secuencial 8 1024 50
	   ./secuencial 8 2048 20
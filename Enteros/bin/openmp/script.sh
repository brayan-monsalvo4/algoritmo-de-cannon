#!/bin/bash

   #2 procesadores

   	echo 0 | sudo tee /sys/devices/system/cpu/cpu2/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu3/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu4/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu5/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu6/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu7/online

   	echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online

	 for k in 4 16 64
	 do
	     for i in 32 64 128 256 512
	      do
		 ./cannon 2 $i $k 100
	      done

	      ./cannon 2 1024 $k 50
	      ./cannon 2 2048 $k 20
	 done

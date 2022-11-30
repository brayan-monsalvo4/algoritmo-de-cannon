
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
		 mpirun -np $k ./cannon2 2 $i $k 100
	      done

	      mpirun -np $k ./cannon2 2 1024 $k 50
	      mpirun -np $k ./cannon2 2 2048 $k 20
	 done


#4 procesadores

	echo 0 | sudo tee /sys/devices/system/cpu/cpu2/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu3/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu6/online
	echo 0 | sudo tee /sys/devices/system/cpu/cpu7/online

   echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu4/online
	echo 1 | sudo tee /sys/devices/system/cpu/cpu5/online


 for k in 4 16 64
 do
     for i in 32 64 128 256 512
      do
         mpirun -np $k ./cannon2 4 $i $k 100
      done

      mpirun -np $k ./cannon2 4 1024 $k 50
      mpirun -np $k ./cannon2 4 2048 $k 20
 done

#8 procesadores

echo 1 | sudo tee /sys/devices/system/cpu/cpu1/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu2/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu3/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu4/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu5/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu6/online
echo 1 | sudo tee /sys/devices/system/cpu/cpu7/online

 for k in 4 16 64
 do
     for i in 32 64 128 256 512
      do
         mpirun -np $k ./cannon2 8 $i $k 100
      done

      mpirun -np $k ./cannon2 8 1024 $k 50
      mpirun -np $k ./cannon2 8 2048 $k 20
 done


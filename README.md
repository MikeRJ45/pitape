# PiTape, Not Pit Ape

A weird little virtual tape driver for Linux that uses a userspace helper app to present the digits of Pi as the data.

Example:
dd if=/dev/pichar of=/home/user/pidigits bs=1 count=1024
This will create the file /home/user/pidigits that contain the first 1024 hexidecimal digits of Pi.

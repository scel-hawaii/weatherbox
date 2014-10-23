# This script does some magic to allow us to read and write from
# serial ports

# Run this 
stty -F /dev/ttyACM2 9600 cs8 cread clocal

# After this runs, you should be able to do this to the serial port!
tail -f /dev/ttyACM2

# Funny enough, what you can also do is 
echo "Hello, World!" >  /dev/ttyACM2



import serial
import time

ARDUINO_USB = "/dev/ttyACM0"

def help():
    print("Available commands:")
    print("  write(int portNumber, float PSI) - Change pressure in port portNumber to pressure PSI.")
    print("  read(int portNumber) - Read the pressure at port portNumber.")
    print("  x - Exit program")

try:
    usb = serial.Serial(ARDUINO_USB, 9600, timeout=2)
except:
    print("ERROR - Could not open USB serial port.  Please check your port name and permissions.")
    print("Exiting program.")
    exit()

print("Enter a command from the keyboard to send to the Arduino.")
while True:
   command = input("Enter command: ")
   if "write" in command:  # read Arduino A0 pin value
      usb.write(command.encode())  # send command to Arduino
      line = usb.readline()  # read input from Arduino
      line = line.decode()  # convert type from bytes to string
      line = line.strip()  # strip extra whitespace characters
      print("Line is: " + line)
      time.sleep(1)
      line2 = usb.readline()  # read input from Arduino
      line2 = line2.decode()  # convert type from bytes to string
      line2 = line2.strip()  # strip extra whitespace characters
      print("Line2 is: " + line2)
      line3 = usb.readline()  # read input from Arduino
      line3 = line3.decode()  # convert type from bytes to string
      line3 = line3.strip()  # strip extra whitespace characters
      print("Line3 is: " + line3)
   elif "read" in command:  # read Arduino A0 pin value
      usb.write(command.encode())  # send command to Arduino
      line = usb.readline()  # read input from Arduino
      line = line.decode()  # convert type from bytes to string
      line = line.strip()  # strip extra whitespace characters
      print("Line is: " + line)
      time.sleep(1)
      line2 = usb.readline()  # read input from Arduino
      line2 = line2.decode()  # convert type from bytes to string
      line2 = line2.strip()  # strip extra whitespace characters
      print("Line2 is: " + line2)
   elif command == "help" or command == "Help":  # print help message
       help()
   elif command == "x":  # exit program
      print("Exiting program.")
      exit()
   else:  # unknown command
      print("Unknown command '" + command + "'.")
      help()

#!/usr/bin/env python

import serial, sys, time

try:
	#mega_serial_port = next(list_ports.grep("Arduino Pro"))
	serial_port = serial.Serial(port="/dev/Marlin",
								baudrate=115200,
								parity=serial.PARITY_NONE,
								stopbits=serial.STOPBITS_ONE,
								bytesize=serial.EIGHTBITS)
	serial_port.isOpen()
	time.sleep(5)
	serial_port.write("M116 a\r")
	serial_port.write("M116 s\r")
	serial_port.write("M116 d\r")
	serial_port.write("M116 f\r")
	time.sleep(3)
	serial_port.close()
except StopIteration:
	print("No Arduino device connected")
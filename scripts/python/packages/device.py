from datetime import datetime as dt
from time import sleep
import serial

class Device:

    ser = None

    def __init__(self, port, baud):
        self.ser = serial.Serial(timeout=1)
        self.ser.baudrate = baud
        self.ser.port = port
        self.ser.open()

        # Turn echo off, helps micro service rapid incoming characters
        self.ser.write('echo off\n'.encode('ascii'))

    def update_datetime(self):
        command = dt.now().strftime('datetime set %Y %m %d %H %M %S %f\n')
        self.ser.write(command.encode('ascii'))
        sleep(1)
        self.ser.flushInput()

    def __del__(self):
        # Turn echo back on
        self.ser.write('echo on\n'.encode('ascii'))
        self.ser.close()
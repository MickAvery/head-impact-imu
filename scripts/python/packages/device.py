from datetime import datetime as dt
from time import sleep
import serial

class Device:
    """
    Represents the IMU device
    ...
    Attributes
    ----------
    ser : Serial object
        serial object to interface with device
    """

    ser = None

    def __init__(self, port, baud):
        """
        Construct a Device object

        Parameters
        ----------
        port : str
            serial port to connect to
        baud : int
            baudrate to communicate to device with
        """
        self.ser = serial.Serial(timeout=1)
        self.ser.baudrate = baud
        self.ser.port = port
        self.ser.open()

        # Turn echo off, helps micro service rapid incoming characters
        self.ser.write('cli echo off\n'.encode('ascii'))

    def update_datetime(self):
        """
        Set device datetime to synchronize with PC time

        Returns
        -------
        None
        """
        command = dt.now().strftime('datetime set %Y %m %d %H %M %S %f\n')
        self.ser.write(command.encode('ascii'))
        sleep(1)
        self.ser.flushInput()

    def __del__(self):
        """
        Delete Device object, making sure to close serial connection

        Returns
        -------
        None
        """
        # Turn echo back on
        self.ser.write('cli echo on\n'.encode('ascii'))
        self.ser.close()
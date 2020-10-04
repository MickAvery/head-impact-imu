from datetime import datetime as dt
from datetime import timedelta as delta
from time import sleep
import csv
import re
import serial

print(
    "\n"
    "**********************\n"
    "* Datetime drift test\n"
    "**********************\n")

ser = serial.Serial()
ser.baudrate = 921600
ser.port = 'COM9'
ser.open()

dataheader = ["EXPECTED", "ACTUAL", "DELTA"]
datapoints = []

# First set datetime on device (NOTE: lots of sleeps cuz for some reason the device can't handle one large write...)
start = dt.now()
end = delta(hours=3, minutes=30) # run test for 3.5 hrs

ser.write('datetime '.encode('ascii'))
sleep(0.001)
ser.write('set '.encode('ascii'))
sleep(0.001)
ser.write((str(start.year)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(start.month)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(start.day)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(start.hour)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(start.minute)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(start.second)+' ').encode('ascii'))
sleep(0.001)
ser.write((str(dt.now().microsecond)).encode('ascii'))
ser.flushInput()
ser.write('\n'.encode('ascii'))
sleep(1)
ser.flushInput()

# start test
while(dt.now() < (start + end)):
    sleep(5)
    print('reading...')

    # get datetime from device
    ser.write('datetime get\n'.encode('ascii'))
    actual = ser.readline().decode('utf-8')
    actual = re.search(r'(\d{4}(-){1}\d+(-){1}\d+ \d{2}(:){1}\d{2}(:){1}\d{2}(.){1}\d{6})', actual).group(1)

    # parse datetime from device
    dt_expected = dt.now()
    dt_actual   = dt.fromisoformat(actual)

    # get timedelta
    diff = dt_expected - dt_actual

    # append all info to datapoints
    datapoints.append([dt_expected, dt_actual, diff])

with open("timedeltas.csv","w+") as my_csv:
    csvWriter = csv.writer(my_csv,delimiter=',')
    csvWriter.writerow(dataheader)
    csvWriter.writerows(datapoints)
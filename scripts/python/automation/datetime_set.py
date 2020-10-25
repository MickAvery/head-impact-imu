import sys, os
sys.path.append(os.path.join(sys.path[0],'..','packages'))

from device import Device

if __name__ == "__main__":

    dev = Device('COM9', 921600)

    dev.update_datetime()

    del dev
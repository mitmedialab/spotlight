import struct
import serial
import csv
import sys
import time

COM_PORT = 'COM11'
SAVE_DIRECTORY = 'C:/Users/patri/Desktop/'

STRUCT_DEF = 'BBiiHHHH'
STRUCT_SIZE = struct.calcsize(STRUCT_DEF)

CURRENT_RES = 0.12158 # uA/bit
BUS_VOLT_RES = 1.25 # mV/bit
SHUNT_VOLT_RES = 2.5 # uV/bit
POWER_RES = CURRENT_RES*25 # uW/bit

current_milli_time = lambda: int(round(time.time() * 1000))

def unpack_spotlight_packet(msg, curr_time):

    #UID, command, angle_1, angle_2, power, current, shuntVoltage, busVoltage = struct.unpack('BBhhHHHH',msg)
    # print(msg)

    if(len(msg) != STRUCT_SIZE):
        return 0

    try:
        unpacked_msg = struct.unpack(STRUCT_DEF,msg) + (curr_time,)
        print(sys.getsizeof(msg))
    except Exception as e:
        print(e)
        print(msg)
        print(sys.getsizeof(msg))
        print('unpack_exception!')

    print(unpacked_msg)
    return unpacked_msg


def main():

    # OPEN COM PORT
    s = serial.Serial(COM_PORT)

    # OPEN LOG FILE
    f = open(SAVE_DIRECTORY + 'spotlight.csv', 'w', newline='')
    writer = csv.writer(f)

    try:
        while(True):
            # GRAB MSG

            # need to do this concatenation since byte object has \n as an acceptable character that isn't "newline"
            received_msg = b''
            while(len(received_msg) < STRUCT_SIZE):
                received_msg += s.readline()

            #grab epoch time
            curr_time = current_milli_time()

            # strip delimiters
            received_msg = received_msg.replace(b'\r\n', b'')

            # UNPACK (remove \r\n with -2)
            print(sys.getsizeof(received_msg))
            # print(received_msg)
            unpacked_msg = unpack_spotlight_packet(received_msg, curr_time)

            # SAVE TO CSV
            if unpacked_msg != 0:
                writer.writerow(unpacked_msg)

            # EVERY FEW SECONDS, SAVE CSV

    except:
            print("GRACEFUL EXIT")

            # EXIT GRACEFULLY IF FORCED

            # SAVE FILE
            f.close()

if __name__ == "__main__":
    main()
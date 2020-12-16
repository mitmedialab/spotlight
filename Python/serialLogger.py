import struct
import serial
import csv
import sys

COM_PORT = 'COM11'
SAVE_DIRECTORY = 'C:/Users/patrick/Desktop/'

STRUCT_DEF = 'BBiiHHHH'
STRUCT_SIZE = struct.calcsize(STRUCT_DEF)

def unpack_spotlight_packet(msg):

    #UID, command, angle_1, angle_2, power, current, shuntVoltage, busVoltage = struct.unpack('BBhhHHHH',msg)
    # print(msg)

    if(len(msg) != STRUCT_SIZE):
        return 0

    try:
        unpacked_msg = struct.unpack(STRUCT_DEF,msg)
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
    f = open(SAVE_DIRECTORY + 'spotlight.csv', 'w')
    writer = csv.writer(f)

    try:
        while(True):
            # GRAB MSG+
            # print('waiting for msg')
            received_msg = s.readline().replace(b'\r\x00\x00', b'').replace(b'\n', b'')

            # UNPACK (remove \r\n with -2)
            print(sys.getsizeof(received_msg))
            print(received_msg)
            unpacked_msg = unpack_spotlight_packet(received_msg)



            # SAVE TO CSV
            # if unpacked_msg != 0:
            #     writer.writerow(unpacked_msg)

            # EVERY FEW SECONDS, SAVE CSV

    except:
            # EXIT GRACEFULLY IF FORCED

            # SAVE FILE
            f.close()

if __name__ == "__main__":
    main()
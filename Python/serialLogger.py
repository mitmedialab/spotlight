import struct
import serial
import csv
import sys
import time

COM_PORT = 'COM6'
SAVE_DIRECTORY = 'C:/Users/patri/Desktop/'

STRUCT_DEF = 'BBiiHHHH'
STRUCT_SIZE = struct.calcsize(STRUCT_DEF)

STRUCT_SENSOR_DEF = "IIffHHHHHHHH" #32
STRUCT_SENSOR_SIZE = struct.calcsize(STRUCT_SENSOR_DEF)

CURRENT_RES = 0.12158 # uA/bit
BUS_VOLT_RES = 1.25 # mV/bit
SHUNT_VOLT_RES = 2.5 # uV/bit
POWER_RES = CURRENT_RES*25 # uW/bit

current_milli_time = lambda: int(round(time.time() * 1000))

def unpack_spotlight_packet(msg, curr_time):

    #UID, command, angle_1, angle_2, power, current, shuntVoltage, busVoltage = struct.unpack('BBhhHHHH',msg)
    # print(msg)

    if(len(msg) == STRUCT_SIZE):
        try:
            unpacked_msg = struct.unpack(STRUCT_DEF,msg) + (curr_time,)
            # print(sys.getsizeof(msg))
        except Exception as e:
            print(e)
            print(msg)
            print(sys.getsizeof(msg))
            print('unpack_exception!')

        print(unpacked_msg)
        return [1, unpacked_msg]
    elif (len(msg) == STRUCT_SENSOR_SIZE):
        try:
            unpacked_msg = struct.unpack(STRUCT_SENSOR_DEF, msg) + (curr_time,)
            # print(sys.getsizeof(msg))
        except Exception as e:
            print(e)
            print(msg)
            print(sys.getsizeof(msg))
            print('unpack_exception!')

        print(unpacked_msg)
        return [2, unpacked_msg]
    else:
        return 0

def main():

    # OPEN COM PORT
    s = serial.Serial(COM_PORT)

    # OPEN LOG FILE
    f = open(SAVE_DIRECTORY + 'spotlight_' + str(int(time.time())) + '.csv', 'w', newline='')
    f_sense = open(SAVE_DIRECTORY + 'spotlight_sensor_' + str(int(time.time())) + '.csv', 'w', newline='')

    writer = csv.writer(f)

    writer_sense = csv.writer(f_sense)

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
            # print(sys.getsizeof(received_msg))
            # print(received_msg)
            type, unpacked_msg = unpack_spotlight_packet(received_msg, curr_time)

            # SAVE TO CSV
            if type == 1:
                writer.writerow(unpacked_msg)
            if type == 2:
                writer_sense.writerow(unpacked_msg)

            # EVERY FEW SECONDS, SAVE CSV

    except:
            print("GRACEFUL EXIT")

            # EXIT GRACEFULLY IF FORCED

            # SAVE FILE
            f.close()
            f_sense.close()

if __name__ == "__main__":
    main()
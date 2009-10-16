#from PIL import Image
from numpy import *
from pylab import *
from random import *
from scipy.misc import *
from PIL import Image
import time
import serial

SOFT_RESET=33

UPDATE_FIRMWARE = 40	# Updates the firmware of the robot 
SAVE_EEPROM = 41	# Takes a backup of the current EEPROM on the computer
RESTORE_EEPROM = 42	# Restores the EEPROM from the backup
WATCHDOG_RESET= 43	# Reboots the processor

GET_ALL=65 
GET_ALL_BINARY=66  
GET_LIGHT_LEFT=67  
GET_LIGHT_CENTER=68  
GET_LIGHT_RIGHT=69  
GET_LIGHT_ALL=70  
GET_IR_LEFT=71  
GET_IR_RIGHT=72  
GET_IR_ALL=73  
GET_LINE_LEFT=74  
GET_LINE_RIGHT=75  
GET_LINE_ALL=76  
GET_STATE=77  
GET_NAME=78
GET_STALL=79  
GET_INFO=80  
GET_DATA=81  

GET_RLE=82  # a segmented and run-length encoded image
GET_IMAGE=83  # the entire 256 x 192 image in YUYV format
GET_WINDOW=84  # the windowed image (followed by which window)
GET_DONGLE_L_IR=85  # number of returned pulses when left emitter is turned on
GET_DONGLE_C_IR=86  # number of returned pulses when center emitter is turned on
GET_DONGLE_R_IR=87  # number of returned pulses when right emitter is turned on
GET_WINDOW_LIGHT=88    # average intensity in the user defined region
GET_BATTERY=89  # battery voltage
GET_SERIAL_MEM=90  # with the address returns the value in serial memory
GET_SCRIB_PROGRAM=91  # with offset, returns the scribbler program buffer
GET_CAM_PARAM=92 # with address, returns the camera parameter at that address
GET_IMAGE_COMPRESSED=93

SET_SINGLE_DATA=96
SET_DATA=97
SET_ECHO_MODE=98
SET_LED_LEFT_ON=99 
SET_LED_LEFT_OFF=100
SET_LED_CENTER_ON=101
SET_LED_CENTER_OFF=102
SET_LED_RIGHT_ON=103
SET_LED_RIGHT_OFF=104
SET_LED_ALL_ON=105
SET_LED_ALL_OFF=106
SET_LED_ALL=107 
SET_MOTORS_OFF=108
SET_MOTORS=109 
SET_NAME=110 
SET_LOUD=111
SET_QUIET=112
SET_SPEAKER=113
SET_SPEAKER_2=114

SET_DONGLE_LED_ON=116   # turn binary dongle led on
SET_DONGLE_LED_OFF=117  # turn binary dongle led off
SET_RLE=118             # set rle parameters 
SET_NAME2=119           # set name2 byte
SET_DONGLE_IR=120       # set dongle IR power
SET_SERIAL_MEM=121      # set serial memory byte
SET_SCRIB_PROGRAM=122   # set scribbler program memory byte
SET_START_PROGRAM=123   # initiate scribbler programming process
SET_RESET_SCRIBBLER=124 # hard reset scribbler
SET_SERIAL_ERASE=125    # erase serial memory
SET_DIMMER_LED=126      # set dimmer led
SET_WINDOW=127          # set user defined window
SET_FORWARDNESS=128     # set direction of scribbler
SET_WHITE_BALANCE=129   # turn on white balance on camera 
SET_NO_WHITE_BALANCE=130 # diable white balance on camera (default)
SET_CAM_PARAM=131       # with address and value, sets the camera parameter at that address


#### Camera Addresses ####

CAM_PID=0x0A
CAM_PID_DEFAULT=0x76

CAM_VER=0x0B
CAM_VER_DEFAULT=0x48

CAM_BRT=0x06
CAM_BRT_DEFAULT=0x80

CAM_EXP=0x10
CAM_EXP_DEFAULT=0x41

CAM_COMA=0x12
CAM_COMA_DEFAULT=0x14
CAM_COMA_WHITE_BALANCE_ON= (CAM_COMA_DEFAULT |  (1 << 2))
CAM_COMA_WHITE_BALANCE_OFF=(CAM_COMA_DEFAULT & ~(1 << 2))

CAM_COMB=0x13
CAM_COMB_DEFAULT=0xA3
CAM_COMB_GAIN_CONTROL_ON= (CAM_COMB_DEFAULT |  (1 << 1))
CAM_COMB_GAIN_CONTROL_OFF=(CAM_COMB_DEFAULT & ~(1 << 1))
CAM_COMB_EXPOSURE_CONTROL_ON= (CAM_COMB_DEFAULT |  (1 << 0))
CAM_COMB_EXPOSURE_CONTROL_OFF=(CAM_COMB_DEFAULT & ~(1 << 0))

def cap(c):
    if (c > 255): 
	return 255
    if (c < 0):
	return 0

    return c

def conf_window(ser, window, X_LOW, Y_LOW, X_HIGH, Y_HIGH, X_STEP, Y_STEP):

    print "Configuring window", window, X_LOW, Y_LOW, X_HIGH, Y_HIGH, X_STEP, Y_STEP
    ser.write(chr(SET_WINDOW))
    ser.write(chr(window)) 
    ser.write(chr(X_LOW)) 
    ser.write(chr(Y_LOW)) 
    ser.write(chr(X_HIGH))
    ser.write(chr(Y_HIGH))
    ser.write(chr(X_STEP))
    ser.write(chr(Y_STEP))


def grab_window(ser, window, lx, ly, ux, uy, xstep, ystep):

    height = (uy - ly + 1) / ystep
    width = (ux - lx + 1) / xstep
    size = width * height
    
    v = zeros(((height + 1), (width + 1)), dtype=uint8)
    v3 = zeros(((height + 1), (width + 1), 3), dtype=uint8)
    
    #done = True
    print "grabbing image = ", window, "width = ", width, "height = ", height
    ser.write(chr(GET_WINDOW))
    ser.write(chr(window))
    
    #print "dimensions = ", ser.read(6)
    line = ''
    while (len(line) < size):
	line += ser.read(size-len(line))
        print "length so far = ", len(line), " waiting for total = ", size

    if (len(line) == width * height):
	i = height
	j = width
	px = 0
 	for i in range(0, height, 1):
 	    for j in range(0, width, 1):
                v[i][j] = ord(line[px])
                px += 1
                
        #create the image from the YUV bayer
	for i in range(0, height, 1):
	    for j in range(3, width, 1):
                if ((j % 4) == 0): #3 #2
                    V = v[i][j]
                    Y = v[i][j-1]
                    U = v[i][j-2]
                elif ((j % 4) == 2): #1 #0
                    U = v[i][j]
                    Y = v[i][j-1]
                    V = v[i][j-2]
                elif ((j % 4) == 3): #2 #1
                    Y = v[i][j]
                    U = v[i][j-1]
                    V = v[i][j-3]
                elif ((j % 4) == 1): #0 #3
                    Y = v[i][j]
                    V = v[i][j-1]
                    U = v[i][j-3]
                    
		U = (U - 128)		
		V = (V - 128)

		v3[i][j][0] = cap(Y + 1.13983 * V)
		v3[i][j][1] = cap(Y - 0.39466*U-0.58060*V)
		v3[i][j][2] = cap(Y + 2.03211*U)
                
        return toimage(v3, high=255, low=0)

def conf_gray_window(ser, window, lx, ly, ux, uy, xstep, ystep):
    print "Configuring gray image on window",  window
    # Y's are on odd pixels
    if (lx % 2)== 0:
        lx += 1
    if (xstep % 2) == 1:
        xstep += 1
    conf_window(ser, window, lx, ly, ux, uy, xstep, ystep)
    height = (uy - ly + 1) / ystep
    width = (ux - lx + 1) / xstep
    return  width * height

def grab_gray_window(ser, window, lx, ly, ux, uy, xstep, ystep):

    # Y's are on odd pixels
    if (lx % 2)== 0:
        lx += 1
    if (xstep % 2) == 1:
        xstep += 1
        
    height = (uy - ly + 1) / ystep
    width = (ux - lx + 1) / xstep
    size = width * height
    
    #done = True
    print "grabbing gray window = ", window, "width = ", width, "height = ", height
    ser.write(chr(GET_WINDOW))
    ser.write(chr(window))
    
    #print "dimensions = ", ser.read(6)
    line = ''
    while (len(line) < size):
	line += ser.read(size-len(line))
        print "length so far = ", len(line), " waiting for total = ", size

    return Image.frombuffer("L", (width, height), line, 'raw', "L", 0, 1)


def grab_image(robotser):

    width = 256
    height = 192
    ser = robotser 
    
    v = zeros(((height + 1), (width + 1)), dtype=uint8)
    v3 = zeros(((height + 1), (width + 1), 3), dtype=uint8)

    #done = True
    print "grabbing image"
    ser.write(chr(GET_IMAGE))
    size= width*height
    line = ''
    while (len(line) < size):
	line += ser.read(size-len(line))
        print "length so far = ", len(line), " waiting for total = ", size

    if (len(line) == width * height):
	i = height
	j = width
	px = 0
 	for i in range(0, height, 1):
 	    for j in range(0, width, 1):
                v[i][j] = ord(line[px])
                px +=1
                
        #create the image from the YUV bayer
	for i in range(0, height, 1):
	    for j in range(3, width, 1):
                if ((j % 4) == 0): #3 #2
                    V = v[i][j]
                    Y = v[i][j-1]
                    U = v[i][j-2]
                elif ((j % 4) == 2): #1 #0
                    U = v[i][j]
                    Y = v[i][j-1]
                    V = v[i][j-2]
                elif ((j % 4) == 3): #2 #1
                    Y = v[i][j]
                    U = v[i][j-1]
                    V = v[i][j-3]
                elif ((j % 4) == 1): #0 #3
                    Y = v[i][j]
                    V = v[i][j-1]
                    U = v[i][j-3]
                    
		U = (U - 128)		
		V = (V - 128)

		v3[i][j][0] = cap(Y + 1.13983 * V)
		v3[i][j][1] = cap(Y - 0.39466*U-0.58060*V)
		v3[i][j][2] = cap(Y + 2.03211*U)
                
        return toimage(v3, high=255, low=0)


def grab_image_rle(robotser):

    width = 256
    height = 192
    ser = robotser 
    
    v = zeros(((height + 1), (width + 1)), dtype=uint8)
    v3 = zeros(((height + 1), (width + 1), 3), dtype=uint8)

    #done = True
    print "grabbing image"
    ser.write(chr(GET_IMAGE))
    size= width*height
    line = ''
    while (len(line) < size):
	line += ser.read(size-len(line))
        print "length so far = ", len(line), " waiting for total = ", size

    if (len(line) == width * height):
	i = height
	j = width
	px = 0
 	for i in range(0, height, 1):
 	    for j in range(0, width, 1):
                v[i][j] = ord(line[px])
                px +=1

        Ys = []
        print type(Ys)
        Us = []
        Vs = []
        
        #create the image from the YUV bayer
	for i in range(0, height, 1):
	    for j in range(3, width, 1):
                if ((j % 4) == 0): #3 #2
                    Vs += [v[i][j]]
                elif ((j % 4) == 1): #0 #3
                    Ys += [v[i][j]]
                elif ((j % 4) == 2): #1 #0                    
                    Us += [v[i][j]]
                elif ((j % 4) == 3): #2 #1
                    Ys += [v[i][j]]

        ythresh = 20
        YRle = []
        last_y = 0
        count = 0
        for y in Ys:
            if abs(last_y - y) > ythresh:
                YRle.append((last_y, count))
                last_y = y
                count = 1
            else:
                count = count + 1

        YRle.append((last_y, count))

        uthresh = 20
        URle = []
        last_u = 0
        count = 0
        for u in Us:        
            if abs(last_u - u) > uthresh:
                URle.append((last_u, count))
                last_u = u
                count = 1
            else:
                count = count + 1
        URle.append((last_u, count))

        vthresh = 20
        VRle = []
        last_v = 0
        count = 0
        for v in Vs:
            if abs(last_v - v) > vthresh:
                VRle.append((last_v, count))
                last_v = v
                count = 1
            else:
                count = count + 1
        VRle.append((last_v, count))

        print "sizes", len(YRle), len(URle), len(VRle)
        print "size", len(YRle) + len(URle) + len(VRle)
        
        return (YRle, URle, VRle)


def grab_and_construct_image(ser, ythresh = 20, uthresh = 20, vthresh = 20):

    width = 256
    height = 192
    
    v = zeros(((height + 1), (width + 1)), dtype=uint8)
    v3 = zeros(((height + 1), (width + 1), 3), dtype=uint8)

    #done = True
    print "grabbing compressed image", ythresh, uthresh, vthresh

    YRle = []
    URle = []
    VRle = []
    
    ser.write(chr(GET_IMAGE_COMPRESSED))
    ser.write(chr(ythresh))
    ser.write(chr(uthresh))
    ser.write(chr(vthresh))
    
    size= width*height

    before = time.time()

    color = 0
    value = 0
    sent = 0
    while (sent < 1):
        byte = ser.read(3)
        color = ord(byte[0])
        value = (ord(byte[1]) << 8) | ord(byte[2])
        if color == 0xFF and value == 0xFFFF:
            sent += 1
        else:
            YRle.append((color, value))
    
    color = 0
    value = 0
    sent = 0
    while (sent < 1):
        byte = ser.read(3)
        color = ord(byte[0])
        value = (ord(byte[1]) << 8) | ord(byte[2])
        if color == 0xFF and value == 0xFFFF:
            sent += 1
        else:
            URle.append((color, value))

    color = 0
    value = 0
    sent = 0
    while (sent < 1):
        byte = ser.read(3)
        color = ord(byte[0])
        value = (ord(byte[1]) << 8) | ord(byte[2])
        if color == 0xFF and value == 0xFFFF:
            sent += 1
        else:
            VRle.append((color, value))

    after = time.time();
    print (after - before), "seconds"
    print "read ys", len(YRle)  
    print "read us", len(URle)    
    print "read vs", len(VRle)

    yi = 0
    vi = 0
    ui = 0

    ycolor = 0
    vcolor = 0
    ucolor = 0
    
    ycount = 0
    vcount = 0
    ucount = 0
    
    #create the image from the YUV bayer
    for i in range(0, height, 1):
        for j in range(0, width, 1):

            if ((j % 4) == 0): #3 #2
                if vcount <= 0:
                    vcolor = VRle[vi][0]
                    vcount = VRle[vi][1]
                    vi = vi + 1                                

                v[i][j] = vcolor
                vcount = vcount - 1
                
                V = v[i][j]
                Y = v[i][j-1]
                U = v[i][j-2]
                
            elif ((j % 4) == 2): #1 #0

                if ucount <= 0:
                    ucolor = URle[ui][0]
                    ucount = URle[ui][1]
                    ui = ui + 1

                v[i][j] = ucolor                        
                ucount = ucount - 1

                U = v[i][j]
                Y = v[i][j-1]
                V = v[i][j-2]
                
            elif ((j % 4) == 3): #2 #1

                if ycount <= 0:
                    ycolor = YRle[yi][0]
                    ycount = YRle[yi][1]                
                    yi = yi + 1
                
                v[i][j] = ycolor
                ycount = ycount - 1

                Y = v[i][j]
                U = v[i][j-1]
                V = v[i][j-3]
                
            elif ((j % 4) == 1): #0 #3

                if ycount <= 0:
                    ycolor = YRle[yi][0]
                    ycount = YRle[yi][1]                
                    yi = yi + 1
                
                v[i][j] = ycolor
                ycount = ycount - 1

                Y = v[i][j]
                V = v[i][j-1]
                U = v[i][j-3]

            U = (U - 128)		
            V = (V - 128)
            
            v3[i][j][0] = cap(Y + 1.13983 * V)
            v3[i][j][1] = cap(Y - 0.39466*U-0.58060*V)
            v3[i][j][2] = cap(Y + 2.03211*U)
            
    return toimage(v3, high=255, low=0)

    

def construct_image(YRle, URle, VRle):

    width = 256
    height = 192
    
    v = zeros(((height + 1), (width + 1)), dtype=uint8)
    v3 = zeros(((height + 1), (width + 1), 3), dtype=uint8)

    yi = 0
    vi = 0
    ui = 0

    ycolor = 0
    vcolor = 0
    ucolor = 0
    
    ycount = 0
    vcount = 0
    ucount = 0
    
    #create the image from the YUV bayer
    for i in range(0, height, 1):
        for j in range(3, width, 1):

            if ((j % 4) == 0): #3 #2
                if vcount <= 0:
                    vcolor = VRle[vi][0]
                    vcount = VRle[vi][1]
                    vi = vi + 1                                

                v[i][j] = vcolor
                vcount = vcount - 1
                
                V = v[i][j]
                Y = v[i][j-1]
                U = v[i][j-2]
                
            elif ((j % 4) == 2): #1 #0

                if ucount <= 0:
                    ucolor = URle[ui][0]
                    ucount = URle[ui][1]
                    ui = ui + 1

                v[i][j] = ucolor                        
                ucount = ucount - 1

                U = v[i][j]
                Y = v[i][j-1]
                V = v[i][j-2]
                
            elif ((j % 4) == 3): #2 #1

                if ycount <= 0:
                    ycolor = YRle[yi][0]
                    ycount = YRle[yi][1]                
                    yi = yi + 1
                
                v[i][j] = ycolor
                ycount = ycount - 1

                Y = v[i][j]
                U = v[i][j-1]
                V = v[i][j-3]
                
            elif ((j % 4) == 1): #0 #3

                if ycount <= 0:
                    ycolor = YRle[yi][0]
                    ycount = YRle[yi][1]                
                    yi = yi + 1
                
                v[i][j] = ycolor
                ycount = ycount - 1

                Y = v[i][j]
                V = v[i][j-1]
                U = v[i][j-3]

            U = (U - 128)		
            V = (V - 128)
            
            v3[i][j][0] = cap(Y + 1.13983 * V)
            v3[i][j][1] = cap(Y - 0.39466*U-0.58060*V)
            v3[i][j][2] = cap(Y + 2.03211*U)
            
    return toimage(v3, high=255, low=0)

    


def conf_gray_image(ser):
    # skip every other pixel
    print "Configuring gray image on window 0"
    conf_window(ser, 0, 1, 0, 255, 191, 2, 2)
    
def grab_gray_image(ser):

    width = 128
    height = 96
    size= width*height

    print "grabbing image size = ", size
    ser.write(chr(GET_WINDOW))
    ser.write(chr(0))
    
    line = ''
    while (len(line) < size):
	line += ser.read(size-len(line))
        print "length so far = ", len(line), " waiting for total = ", size

    return Image.frombuffer("L", (width, height), line, 'raw', "L", 0, 1)

def conf_rle(ser,
             delay = 90, smooth_thresh = 4,
             y_low=0, y_high=254,
             u_low=51, u_high=136,
             v_low=190, v_high=254):

    print "Configuring blobs"
    ser.write(chr(SET_RLE))
    ser.write(chr(delay))
    ser.write(chr(smooth_thresh))
    ser.write(chr(y_low)) 
    ser.write(chr(y_high))
    ser.write(chr(u_low)) 
    ser.write(chr(u_high))
    ser.write(chr(v_low)) 
    ser.write(chr(v_high))

    
def grab_rle(ser):

    width = 256
    height = 192    
    blobs = zeros(((height + 1), (width + 1)), dtype=uint8)
    
    line = ''
    ser.write(chr(GET_RLE))
    size=ord(ser.read(1))
    size = (size << 8) | ord(ser.read(1))
    print "Grabbing RLE image size =", size
    line =''
    while (len(line) < size):
        line+=ser.read(size-len(line))
    
    px = 0
    counter = 0
    val = 128
    inside = True
    for i in range(0, height, 1):
        for j in range(0, width, 4):
            if (counter < 1 and px < len(line)):
                counter = ord(line[px])    	
                px += 1
                counter = (counter << 8) | ord(line[px])    	
                px += 1

                if (inside):
                    val = 0
                    inside = False
                else:
                    val = 255
                    inside = True

            for z in range(0,4):
                blobs[i][j+z] = val
            counter -= 1
                
    return toimage(blobs, high=255, low=0)

def grab_rle_on(ser):

    width = 256
    height = 192    
    blobs = zeros(((height + 1), (width + 1)), dtype=uint8)

    on_pxs = []
    
    line = ''
    ser.write(chr(GET_RLE))
    size=ord(ser.read(1))
    size = (size << 8) | ord(ser.read(1))
    #print "Grabbing RLE image size =", size
    line =''
    while (len(line) < size):
        line+=ser.read(size-len(line))

    px = 0
    counter = 0
    val = 128
    inside = True
    for i in range(0, height, 1):
        for j in range(0, width, 4):            
            if (counter < 1 and px < len(line)):
                counter = ord(line[px])    	
                px += 1
                counter = (counter << 8) | ord(line[px])    	
                px += 1

                if (inside):
                    val = 0
                    inside = False
                else:
                    val = 255
                    inside = True

            for z in range(0,4):
                blobs[i][j+z] = val
                if (inside):
                    on_pxs += [[j+z, i]]
            counter -= 1
            
    return on_pxs


def read_2byte(ser):
    hbyte = ord(ser.read(1))
    lbyte = ord(ser.read(1))
    lbyte = (hbyte << 8) | lbyte
    return lbyte

def read_4byte(ser):
    byte3 = ord(ser.read(1))
    byte2 = ord(ser.read(1))
    byte1 = ord(ser.read(1))
    byte0 = ord(ser.read(1))
    byte = (byte0) | (byte1 << 8) | (byte2 << 16) |  (byte3 << 24)
    return byte

def write_2byte(ser, value):
    ser.write(chr((value >> 8) & 0xFF))
    ser.write(chr(value & 0xFF))

def read_mem(ser, page, offset):
    ser.write(chr(GET_SERIAL_MEM))
    write_2byte(ser, page)
    write_2byte(ser, offset)
    return ord(ser.read(1))

def write_mem(ser, page, offset, byte):
    ser.write(chr(SET_SERIAL_MEM))
    write_2byte(ser, page)
    write_2byte(ser, offset)
    ser.write(chr(byte))

def erase_mem(ser, page):
    ser.write(chr(SET_SERIAL_ERASE))
    write_2byte(ser, page)

def get_ir_left(ser):
    ser.write(chr(GET_DONGLE_L_IR))
    return read_2byte(ser)
   
def get_ir_right(ser):
    ser.write(chr(GET_DONGLE_R_IR))
    return read_2byte(ser)

def get_ir_middle(ser):
    ser.write(chr(GET_DONGLE_C_IR))
    return read_2byte(ser)

def get_battery(ser):
    ser.write(chr(GET_BATTERY))
    return read_2byte(ser)

def set_ir_power(ser, power):
    ser.write(chr(SET_DONGLE_IR))
    ser.write(chr(power))

def set_led1_on(ser):
    ser.write(chr(SET_DONGLE_LED_ON))

def set_led1_off(ser):
    ser.write(chr(SET_DONGLE_LED_OFF))

def set_led2(ser, value):
    ser.write(chr(SET_DIMMER_LED))
    ser.write(chr(value))

def get_window_avg(ser, window):
    ser.write(chr(GET_WINDOW_LIGHT))
    ser.write(chr(window))
    return read_4byte(ser)

def set_forwardness(ser, direction):
    ser.write(chr(SET_FORWARDNESS))
    ser.write(chr(direction))

def set_scribbler_memory(ser, offset, byte):
    ser.write(chr(SET_SCRIB_PROGRAM))
    write_2byte(ser, offset)
    ser.write(chr(byte))

def set_scribbler_start_program(ser, size):
    ser.write(chr(SET_START_PROGRAM))
    write_2byte(ser, size)
        
def set_reset_scribbler(ser):
    ser.write(chr(SET_RESET_SCRIBBLER))

def set_cam_param(ser, addr, byte):
    ser.write(chr(SET_CAM_PARAM))
    ser.write(chr(addr))
    ser.write(chr(byte))

def get_cam_param(ser, addr):
    ser.write(chr(GET_CAM_PARAM))
    ser.write(chr(addr))
    return ord(ser.read(1))


def set_no_white_balance(ser):
    ser.write(chr(SET_NO_WHITE_BALANCE))

def set_white_balance(ser):
    ser.write(chr(SET_WHITE_BALANCE))



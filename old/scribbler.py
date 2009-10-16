"""
Myro code for the Scribbler robot from Parallax
(c) 2007, Institute for Personal Robots in Education
http://roboteducation.org/
Distributed under a Shared Source License
"""

__REVISION__ = "$Revision: 1131 $"
__AUTHOR__   = "Keith O'Hara and Doug Blank"

import time, string
import os
try:
    import serial
except:
    print "WARNING: pyserial not loaded: scribbler won't work!"
from myro import Robot, ask
from myro.graphics import _askQuestion, Picture, rgb2yuv
import myro.globvars
import cStringIO
# needed for new camera dongle
try:
    from numpy import array
except:
    pass

class BufferedRead:
    def __init__(self, serial, size, start = 1):
        self.serial = serial
        self.size = size
        if start:
            self.data = self.serial.read(size)
        else:
            self.data = ""
    def __getitem__(self, position):
        """ Return an element of the string """
        while position >= len(self.data):
            #self.data += self.serial.read(self.size - len(self.data))
            self.data += self.serial.read(self.size - len(self.data))
            #print "      length so far = ", len(self.data), " waiting for total = ", self.size
        return self.data[position]
    def __len__(self):
        """ Lie. Tell them it is this long. """
        return self.size

def _commport(s):
    if type(s) == int: return 1
    if type(s) == str:
        s = s.replace('\\', "")
        s = s.replace('.', "")
        if s.lower().startswith("com") and s[3:].isdigit():
            return 1
        if s.startswith("/dev/"):
            return 1
    return 0

def isTrue(value):
    """
    Returns True if value is something we consider to be "on".
    Otherwise, return False.
    """
    if type(value) == str:
        return (value.lower() == "on")
    elif value: return True
    return False

class Scribbler(Robot):
    SOFT_RESET=33
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
    GET_NAME1=78
    GET_NAME2=64
    GET_STALL=79  
    GET_INFO=80  
    GET_DATA=81  

    GET_PASS1=50
    GET_PASS2=51

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

    GET_BLOB=95

    SET_PASS1=55
    SET_PASS2=56
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
    SET_NAME1=110 
    SET_NAME2=119           # set name2 byte
    SET_LOUD=111
    SET_QUIET=112
    SET_SPEAKER=113
    SET_SPEAKER_2=114

    SET_DONGLE_LED_ON=116   # turn binary dongle led on
    SET_DONGLE_LED_OFF=117  # turn binary dongle led off
    SET_RLE=118             # set rle parameters 
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

    GET_JPEG_GRAY_HEADER=135
    GET_JPEG_GRAY_SCAN=136
    GET_JPEG_COLOR_HEADER=137
    GET_JPEG_COLOR_SCAN=138

    SET_PASS_N_BYTES=139
    GET_PASS_N_BYTES=140
    GET_PASS_BYTES_UNTIL=141

    GET_VERSION=142

    PACKET_LENGTH     =  9
    
    def __init__(self, serialport = None, baudrate = 38400):
	print "inside scribbler.py.Scribbler.__init__\n"

	####SKLAR Robot.__init__(self)
	print "after Robot.__init__\n"

        #### Camera Addresses ####
        self.CAM_PID=0x0A
        self.CAM_PID_DEFAULT=0x76
    
        self.CAM_VER=0x0B
        self.CAM_VER_DEFAULT=0x48
    
        self.CAM_BRT=0x06
        self.CAM_BRT_DEFAULT=0x80
    
        self.CAM_EXP=0x10
        self.CAM_EXP_DEFAULT=0x41
    
        self.CAM_COMA=0x12
        self.CAM_COMA_DEFAULT=0x14
        self.CAM_COMA_WHITE_BALANCE_ON= (self.CAM_COMA_DEFAULT |  (1 << 2))
        self.CAM_COMA_WHITE_BALANCE_OFF=(self.CAM_COMA_DEFAULT & ~(1 << 2))
    
        self.CAM_COMB=0x13
        self.CAM_COMB_DEFAULT=0xA3
        self.CAM_COMB_GAIN_CONTROL_ON= (self.CAM_COMB_DEFAULT |  (1 << 1))
        self.CAM_COMB_GAIN_CONTROL_OFF=(self.CAM_COMB_DEFAULT & ~(1 << 1))
        self.CAM_COMB_EXPOSURE_CONTROL_ON= (self.CAM_COMB_DEFAULT |  (1 << 0))
        self.CAM_COMB_EXPOSURE_CONTROL_OFF=(self.CAM_COMB_DEFAULT & ~(1 << 0))

        self.ser = None
        self.requestStop = 0
        self.debug = 0
        self._lastTranslate = 0
        self._lastRotate    = 0
        self._volume = 0
	print "serialport=%s\n" % serialport
        if serialport == None:
            if 'MYROROBOT'in os.environ:
                serialport = os.environ['MYROROBOT']
                print "Connecting to", serialport
            else:
                serialport = ask("Port", useCache = 1)
        # Deal with requirement that Windows "COM#" names where # >= 9 needs to
        # be in the format "\\.\COM#"
        hasPort = True
        if type(serialport) == str and serialport.lower().startswith("com"):
            portnum = int(serialport[3:])
        elif isinstance(serialport, int): #allow integer input
            portnum = serialport
        else: hasPort = False
        if hasPort:
            if portnum >= 10:
                serialport = r'\\.\COM%d' % (portnum)
        self.serialPort = serialport
        self.baudRate = baudrate
        self.open()
        
        myro.globvars.robot = self
#        self._fudge = range(4)
#        self._oldFudge = range(4)
        self.dongle = None
#        info = self.getInfo()
#        if "fluke" in info.keys():
#            self.dongle = info["fluke"]
#            print "You are using fluke firmware", info["fluke"]
#        elif "dongle" in info.keys():
#            self.dongle = info["dongle"]
#            print "You are using fluke firmware", info["dongle"]
#        if self.dongle != None:
#            # Turning on White Balance, Gain Control, and Exposure Control
#            self.set_cam_param(self.CAM_COMA, self.CAM_COMA_WHITE_BALANCE_ON)
#            self.set_cam_param(self.CAM_COMB, self.CAM_COMB_GAIN_CONTROL_ON | self.CAM_COMB_EXPOSURE_CONTROL_ON)
#            # Config grayscale on window 0, 1, 2
#            conf_gray_window(self.ser, 0, 2,   0, 128, 191, 1, 1)
#            conf_gray_window(self.ser, 1, 64,  0, 190, 191, 1, 1)
#            conf_gray_window(self.ser, 2, 128, 0, 254, 191, 1, 1)
#            set_ir_power(self.ser, 135)
#            self.conf_rle(delay = 90, smooth_thresh = 4,
#                          y_low=0, y_high=255,
#                          u_low=51, u_high=136,
#                          v_low=190, v_high=255)
#
#        self.robotinfo = {}
#        if "robot" in info.keys():
#            self.robotinfo = info["robot"]
#            if "robot-version" in info.keys():
#                print "You are using scribbler firmware", info["robot-version"]
#            elif "api" in info.keys():
#                print "You are using scribbler firmware", info["api"]
#            self.restart()
#            self.loadFudge()


    def search(self):
        answer = _askQuestion(title="Search for " + self.serialPort,
                             question="Press the red resest button on the robot\nPress OK when ready to search",
                             answers = ["OK", "Cancel"])
        if answer != "OK":
            raise KeyboardInterrupt
        for x in range(1, 21):
            if x >= 10:
                port = r'\\.\COM%d' % x
            else:
                port = "COM" + str(x)
            prettyPort = "COM" + str(x)
            print "Searching on port %s for robot named '%s'..." % (prettyPort, self.serialPort)
            try:
                self.ser = serial.Serial(port, timeout=10)
            except KeyboardInterrupt:
                raise
            except serial.SerialException:
                print "   Serial element not found. If this continues, remove/replace serial device..."
                continue
            self.ser.baudrate = self.baudRate
            # assume that it has been running for at least a second!
            time.sleep(1)
            lines = self.ser.readlines()
            lines = ''.join(lines)
            if ("IPRE" in lines):
                position = lines.index("IPRE")
                name = lines[position+4:position + 9 + 4]
                name = name.replace("\x00", "")
                name = name.strip()
                s = port.replace('\\', "")
                s = s.replace('.', "")
                print "   Found robot named", name, "on port", s, "!"
                if name == self.serialPort:
                    self.serialPort = port
                    self.ser.timeout = 10
                    s = self.serialPort.replace('\\', "")
                    s = s.replace('.', "")
                    _askQuestion("You can use \"%s\" from now on, like this:\n   initialize(\"%s\")" %
                                (s, s), answers=["Ok"])
                    return
                else:
                    self.ser.close()
        raise ValueError("Couldn't find robot named '%s'" % self.serialPort)
    
    def open(self):
        try:
	    print "trying to open serial port\n"
            if self.serialPort == myro.globvars.robot.ser.portstr:
                myro.globvars.robot.ser.close()
                print "Closing serial port..."
                time.sleep(1)
        except KeyboardInterrupt:
            raise
        except:
            pass
        if not _commport(self.serialPort):
            self.search()
        else:
            while 1:
                try:
                    print "trying2"
                    self.ser = serial.Serial(self.serialPort, timeout = 10) 
                    break
                except KeyboardInterrupt:
                    raise
                except serial.SerialException:
                    print "   Serial element not found. If this continues, remove/replace serial device..."
                    try:
                        self.ser.close()
                    except KeyboardInterrupt:
                        raise
                    except:
                        pass
                    try:
                        del self.ser
                    except KeyboardInterrupt:
                        raise
                    except:
                        pass
                    time.sleep(1)
                except:
                    print "Waiting on port...", self.serialPort
                    try:
                        self.ser.close()
                    except KeyboardInterrupt:
                        raise
                    except:
                        pass
                    try:
                        del self.ser
                    except KeyboardInterrupt:
                        raise
                    except:
                        pass
                    time.sleep(1)
        print "looks good"
        self.ser.baudrate = self.baudRate
        #self.restart()
	print "opened a-okay\n"
	print "baudrate=%s " % self.ser.baudrate
	print "timeout=%s  " % self.ser.timeout
	print "bytesize=%s " % self.ser._bytesize
	print "stopbits=%s " % self.ser._stopbits
	print "parity=%s   " % self.ser._parity
	print "flowctl=%s  " % self.ser._xonxoff
	print "rtscts=%s   " % self.ser._rtscts
	#print "recbyth=%s  " % self.ser.ReceivedBytesThreshold
	#print "readto =%s  " % self.ser._port_handle.ReadTimeout


    def close(self):
        self.ser.close()

    def manual_flush(self):
        old = self.ser.timeout
        self.ser.setTimeout(.5)
        l = 'a'
        count = 0;
        while (len(l) != 0 and count < 50000):
            l = self.ser.read(1)
            count += len(l)
        self.ser.setTimeout(old)

    def restart(self):
        self.manual_flush()
        self.setEchoMode(0) # send command to get out of broadcast; turn off echo
        time.sleep(.25)               # give it some time
        while 1:
            self.ser.flushInput()         # flush "IPREScribby"...
            self.ser.flushOutput()
            time.sleep(1.2)       # give it time to see if another IPRE show up
            if self.ser.inWaiting() == 0: # if none, then we are out of here!
                break
            print "Waking robot from sleep..."
            self.setEchoMode(0) # send command to get out of broadcast; turn off echo
            time.sleep(.25)               # give it some time
        self.ser.flushInput()
        self.ser.flushOutput()
        self.stop()
        self.set("led", "all", "off")
        self.beep(.03, 784)
        self.beep(.03, 880)
        self.beep(.03, 698)
        self.beep(.03, 349)
        self.beep(.03, 523)
        name = self.get("name")
        print "Hello, I'm %s!" % name

    def beep(self, duration, frequency, frequency2 = None):

        self.lock.acquire() #print "locked acquired"
        
        old = self.ser.timeout
        self.ser.setTimeout(duration+2)

        if frequency2 == None:
            self._set_speaker(int(frequency), int(duration * 1000))
        else:
            self._set_speaker_2(int(frequency), int(frequency2), int(duration * 1000))

        v = self.ser.read(Scribbler.PACKET_LENGTH + 11)

        if self.debug:
            print map(lambda x:"0x%x" % ord(x), v)

        self.ser.setTimeout(old)
        self.lock.release()
        
    def get(self, sensor = "all", *position):
        sensor = sensor.lower()
        if sensor == "config":
            if self.dongle == None:
                return {"ir": 2, "line": 2, "stall": 1, "light": 3}
            else:
                return {"ir": 2, "line": 2, "stall": 1, "light": 3,
                        "battery": 1, "obstacle": 3, "bright": 3}
        elif sensor == "stall":
            retval = self._get(Scribbler.GET_ALL, 11) # returned as bytes
            self._lastSensors = retval # single bit sensors
            return retval[10]
        elif sensor == "forwardness":
            if read_mem(self.ser, 0, 0) != 0xDF:
                retval = "fluke-forward"
            else:
                retval = "scribbler-forward"
            return retval
        elif sensor == "startsong":
            #TODO: need to get this from flash memory
            return "tada"
        elif sensor == "version":
            #TODO: just return this version for now; get from flash
            return __REVISION__.split()[1]
        elif sensor == "data":
            return self.getData(*position)
        elif sensor == "info":
            return self.getInfo(*position)
        elif sensor == "name":
            c = self._get(Scribbler.GET_NAME1, 8)
            c += self._get(Scribbler.GET_NAME2, 8)
            c = string.join([chr(x) for x in c if "0" <= chr(x) <= "z"], '').strip()
            return c
        elif sensor == "password":
            c = self._get(Scribbler.GET_PASS1, 8)
            c += self._get(Scribbler.GET_PASS2, 8)
            c = string.join([chr(x) for x in c if "0" <= chr(x) <= "z"], '').strip()
            return c
        elif sensor == "volume":
            return self._volume
        elif sensor == "battery":
            return self.getBattery()
        elif sensor == "blob":
            return self.getBlob()
        else:
            if len(position) == 0:
                if sensor == "light":
                    return self._get(Scribbler.GET_LIGHT_ALL, 6, "word")
                elif sensor == "line":
                    return self._get(Scribbler.GET_LINE_ALL, 2)
                elif sensor == "ir":
                    return self._get(Scribbler.GET_IR_ALL, 2)
                elif sensor == "obstacle":
                    return [self.getObstacle("left"), self.getObstacle("center"), self.getObstacle("right")]
                elif sensor == "bright":
                    return [self.getBright("left"), self.getBright("middle"), self.getBright("right") ]
                elif sensor == "all":
                    retval = self._get(Scribbler.GET_ALL, 11) # returned as bytes
                    self._lastSensors = retval # single bit sensors
                    if self.dongle == None:
                        return {"light": [retval[2] << 8 | retval[3], retval[4] << 8 | retval[5], retval[6] << 8 | retval[7]],
                                "ir": [retval[0], retval[1]], "line": [retval[8], retval[9]], "stall": retval[10]}
                    else:
                        return {"light": [retval[2] << 8 | retval[3], retval[4] << 8 | retval[5], retval[6] << 8 | retval[7]],
                                "ir": [retval[0], retval[1]], "line": [retval[8], retval[9]], "stall": retval[10],
                                "obstacle": [self.getObstacle("left"), self.getObstacle("center"), self.getObstacle("right")],
                                "bright": [self.getBright("left"), self.getBright("middle"), self.getBright("right")],
                                "blob": self.getBlob(),
                                "battery": self.getBattery(),
                                }
                else:                
                    raise ("invalid sensor name: '%s'" % sensor)
            retvals = []
            for pos in position:
                if sensor == "light":
                    values = self._get(Scribbler.GET_LIGHT_ALL, 6, "word")
                    if pos in [0, "left"]:
                        retvals.append(values[0])
                    elif pos in [1, "middle", "center"]:
                        retvals.append(values[1])
                    elif pos in [2, "right"]:
                        retvals.append(values[2])
                    elif pos == None or pos == "all":
                        retvals.append(values)
                elif sensor == "ir":
                    values = self._get(Scribbler.GET_IR_ALL, 2)                    
                    if pos in [0, "left"]:
                        retvals.append(values[0])
                    elif pos in [1, "right"]:
                        retvals.append(values[1])
                    elif pos == None or pos == "all":
                        retvals.append(values)
                elif sensor == "line":
                    values = self._get(Scribbler.GET_LINE_ALL, 2)
                    if pos in [0, "left"]:
                        retvals.append(values[0])
                    elif pos in [1, "right"]:
                        retvals.append(values[1])
                elif sensor == "obstacle":
                    return self.getObstacle(pos)
                elif sensor == "bright":
                    return self.getBright(pos)
                elif sensor == "picture":
                    return self.takePicture(pos)
                else:
                    raise ("invalid sensor name: '%s'" % sensor)
            if len(retvals) == 0:
                return None
            elif len(retvals) == 1:
                return retvals[0]
            else:
                return retvals

    def getData(self, *position):
        if len(position) == 0: 
            return self._get(Scribbler.GET_DATA, 8)
        else:   
            retval = []               
            for p in position:
                retval.append(self._get(Scribbler.GET_DATA, 8)[p])
            if len(retval) == 1:
                return retval[0]
            else:
                return retval

    def getInfo(self, *item):
        #retval = self._get(Scribbler.GET_INFO, mode="line")

        oldtimeout = self.ser.timeout        
        self.ser.setTimeout(4)
        
        #self.ser.flushInput()
        #self.ser.flushOutput()
        
        self.manual_flush()
        # have to do this twice since sometime the first echo isn't
        # echoed correctly (spaces) from the scribbler
        self.ser.write(chr(Scribbler.GET_INFO) + (' ' * 8))
        retval = self.ser.readline()
        #print "Got", retval

        time.sleep(.1)
        
        self.ser.write(chr(Scribbler.GET_INFO) + (' ' * 8))
        retval = self.ser.readline()
        #print "Got", retval
        
        # remove echoes
        if retval == None or len(retval) == 0:
            return {}
        
        if retval[0] == 'P' or retval[0] == 'p':
            retval = retval[1:]
        
        if retval[0] == 'P' or retval[0] == 'p':
            retval = retval[1:]

        self.ser.setTimeout(oldtimeout)

        retDict = {}
        for pair in retval.split(","):
            if ":" in pair:            
                it, value = pair.split(":")
                retDict[it.lower().strip()] = value.strip()
        if len(item) == 0:  
            return retDict
        else:               
            retval = []
            for it in item:
                retval.append(retDict[it.lower().strip()])
            if len(retval) == 1:
                return retval[0]
            else:
                return retval

    ########################################################## Dongle Commands


# the set_blob_yuv function will set the YUV colorspace for the blob
# detection on the fluke based upon the average hue of a rectangle of
# pixels in an image.
# The GUI allows users to drag to select a rectangle in an image, and 
# the average HUE of the pixels in that rectangle are used to set the
# U and V parameters (hue/color parameters) of the blob detecting code
# on the Lance Fluke (dongle).
#
# We set the Y value (Luminance) to 0-254 because we will accept pixels
# of all intensities.
#
# The averaging code (used to find AVG/StdDev and calculate the average
# range of hue values was based upon a donation from the C++ version of
# the MYRO API, donated by redwar15@cs.utk.edu

    def set_blob_yuv(self, picture, x1, y1, x2, y2):
        from math import sqrt
        #Find min/max of rectangle.
        xs = [x1,x2]
        ys = [y1,y2]
        xs.sort()
        ys.sort()
	
	#set up variables to hold counts and accumulations:
        totalY = 0.0
        totalU = 0.0
        totalV = 0.0
       
        ySamples = []
        uSamples = []
        vSamples = [] 
        
        for i in range(xs[0], xs[1], 1):
           for j in range(ys[0], ys[1], 1):
              r,g,b = picture.getPixel(i,j).getRGB()
              y,u,v = rgb2yuv(r,g,b)
              totalY = totalY + y
              totalU = totalU + u
              totalV = totalV + v
              ySamples.append(y)
              uSamples.append(u)
              vSamples.append(v)

        count = len(ySamples)
        yMean = totalY / count
        uMean = totalU / count
        vMean = totalV / count


	# The standard deviation of a random variable with a normal 
        # distribution is the root-mean-square (RMS) deviation of its 
        # values from their mean.
	sY = 0.0
        sU = 0.0
        sV = 0.0

        for i in range(0,len(ySamples)):
           sY = sY + (ySamples[i] - yMean)**2
           sU = sU + (uSamples[i] - uMean)**2
           sV = sV + (vSamples[i] - vMean)**2

        sY = sqrt( sY / count)
        sU = sqrt( sU / count)
        sV = sqrt( sV / count)

        # Select the U/V bounding box based upon stdMod stdDev
        # from the mean, with approripate
        # min/max values to fit in an 8 bit register.
	#
	stdMod = 3.0

        minU = max(0, (uMean - sU*stdMod)    )
        maxU = min(255, (uMean + sU*stdMod)  )
        minV = max(0, (vMean - sV*stdMod) )
        maxV = min(255, (vMean + sV*stdMod) )
        minU = int(minU)
        maxU = int(maxU)
        minV = int(minV)
        maxV = int(maxV)


	#Note that we use the default values for
        #several parameters, most importantly the Y value
        # defaults to a range of 0-254
        self.conf_rle( u_low=minU, u_high=maxU,
                      v_low=minV, v_high=maxV)

        # Return a tupal of parameters suitable for the configureBlob
        # function, to be shown to the user.

        return ( 0, 254, minU, maxU, minV, maxV) 












    def configureBlob(self,
                      y_low=0,  y_high=255,
                      u_low=0,  u_high=255,
                      v_low=0,  v_high=255,
                      smooth_thresh=4):        
        self.conf_rle(y_low=y_low, y_high=y_high,
                      u_low=u_low, u_high=u_high,
                      v_low=v_low, v_high=v_high,
                      smooth_thresh=smooth_thresh)

# conf_rle   - Sets parameters for the Run Length Encoded blob image
#		returned by takePicture("blob")
#
# Y,U,V high/low parameters configure a bounding box (in YUV color space)
# of pixels that are "on" or white. All other pixels are off or black.
#
# Note - The delay parameter should always be 90. it's an internal tuning 
# parameter, and Keith says Bad Things(TM) happen if it's not 90.
#
# The smooth_thresh parameter tells how many pixels must be black/white
# before the Run Length Encoding (of blob images) recognizes the change
# and sends a run of the opposite bit. This makes the blob images look 
# slightly streaky, but greatly increases the efficiency of the RLE compression
# by eliminating minor noise.

    def conf_rle(self, delay = 90, smooth_thresh = 4,
                 y_low=0, y_high=254,
                 u_low=51, u_high=136,
                 v_low=190, v_high=254):

        # Force all parameters to be INT's
        y_low = int(y_low)
        y_high = int(y_high)
        u_low  = int(u_low)
        u_high = int(u_high)
        v_low  = int(v_low)
        v_high = int(v_high)
        delay = int(delay)
        smooth_thresh = int(smooth_thresh)

        if self.debug:
            print "configuring RLE", delay, smooth_thresh, y_low, y_high, u_low, u_high, v_low, v_high
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_RLE))
            self.ser.write(chr(delay))
            self.ser.write(chr(smooth_thresh))
            self.ser.write(chr(y_low)) 
            self.ser.write(chr(y_high))
            self.ser.write(chr(u_low)) 
            self.ser.write(chr(u_high))
            self.ser.write(chr(v_low)) 
            self.ser.write(chr(v_high))
        finally:
            self.lock.release()

    def read_uint32(self):
        buf = self.ser.read(4)
        return ord(buf[0]) + ord(buf[1]) * 256 + ord(buf[2]) * 65536 + ord(buf[3]) * 16777216
    
    def read_jpeg_scan(self):
        bytes = ''
        last_byte = 0
        while True:
            byte = self.ser.read(1)
            bytes += byte
            
            if last_byte == chr(0xff) and byte == chr(0xd9):
                # End-of-image marker
                break

            last_byte = byte

        bm0 = self.read_uint32();   # Start
        bm1 = self.read_uint32();   # Read
        bm2 = self.read_uint32();   # Compress

        if self.debug:
            print "got image"
            freq = 60e6
            print '%.3f %.3f' % (((bm1 - bm0) / freq), ((bm2 - bm1) / freq))
        
        return bytes
    
    def read_jpeg_header(self):
        buf = self.ser.read(2)
        len = ord(buf[0]) + ord(buf[1]) * 256
        return self.ser.read(len)
    
    color_header = None
    def grab_jpeg_color(self, reliable):
        try:
            self.lock.acquire()
            if self.color_header == None:
                self.ser.write(chr(self.GET_JPEG_COLOR_HEADER))
                self.color_header = self.read_jpeg_header()
            
            self.ser.write(chr(self.GET_JPEG_COLOR_SCAN))
            self.ser.write(chr(reliable))
            jpeg = self.color_header + self.read_jpeg_scan()
        finally:
            self.lock.release()
        return jpeg
    
    gray_header = None
    def grab_jpeg_gray(self, reliable):
        try:
            self.lock.acquire()
            if self.gray_header == None:
                self.ser.write(chr(self.GET_JPEG_GRAY_HEADER))
                self.gray_header = self.read_jpeg_header()
            
            self.ser.write(chr(self.GET_JPEG_GRAY_SCAN))
            self.ser.write(chr(reliable))
            jpeg = self.gray_header + self.read_jpeg_scan()
        finally:
            self.lock.release()
        return jpeg

    # for older versions of fluke without jpeg
    # use this lookup table to decide what picture to grab
    image_codes = {"jpeg": "color",
                   "jpeg-fast": "color",
                   "grayjpeg": "gray",
                   "grayjpeg-fast": "gray"}
    
    def takePicture(self, mode="jpeg"):
        width = 256
        height = 192
        p = Picture()

        if self.dongle:
            version = map(int, self.dongle.split("."))
        else:
            version = [1, 0, 0]
                
        if version < [2, 7, 8]:
            if mode in self.image_codes:
                mode = self.image_codes[mode]
            
        if mode == "color":
            a = self._grab_array()
            p.set(width, height, a)
        elif mode == "jpeg":
            jpeg = self.grab_jpeg_color(1)
            stream = cStringIO.StringIO(jpeg)  
            p.set(width, height, stream, "jpeg")
        elif mode == "jpeg-fast":
            jpeg = self.grab_jpeg_color(0)
            stream = cStringIO.StringIO(jpeg)  
            p.set(width, height, stream, "jpeg")
        elif mode == "grayjpeg":
            jpeg = self.grab_jpeg_gray(1)
            stream = cStringIO.StringIO(jpeg)  
            p.set(width, height, stream, "jpeg")
        elif mode == "grayjpeg-fast":
            jpeg = self.grab_jpeg_gray(0)
            stream = cStringIO.StringIO(jpeg)  
            p.set(width, height, stream, "jpeg")
        elif mode in ["gray", "grey"]:
            conf_window(self.ser, 0, 1, 0, 255, 191, 2, 2)
            a = self._grab_gray_array()
            conf_gray_window(self.ser, 0, 2, 0,    128, 191, 1, 1)
            p.set(width, height, a, "gray")
        elif mode == "blob":
            a = self._grab_blob_array()
            p.set(width, height, a, "blob")
        return p

    def _grab_blob_array(self):
        width = 256
        height = 192    
        blobs = array([0] * (height * width), 'B') # zeros(((height + 1), (width + 1)), dtype=uint8)
        line = ''
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.GET_RLE))
            size=ord(self.ser.read(1))
            size = (size << 8) | ord(self.ser.read(1))
            if self.debug:
                print "Grabbing RLE image size =", size
            line =''
            while (len(line) < size):
                line+=self.ser.read(size-len(line))
        finally:
            self.lock.release()
            
        px = 0
        counter = 0
        val = 128
        inside = True
        for i in range(height):
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
                    blobs[i * width + j+z] = val
                counter -= 1
        return blobs

    def _grab_gray_array(self):
        width = 128
        height = 96
        size= width*height
        #print "grabbing image size = ", size
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.GET_WINDOW))
            self.ser.write(chr(0))
            line = ''
            while (len(line) < size):
                line += self.ser.read(size-len(line))
                #print "length so far = ", len(line), " waiting for total = ", size
        finally:
            self.lock.release()
            
        line = quadrupleSize(line, width)
        return line

    def _grab_array(self):
        width = 256
        height = 192
        buffer = array([0] * (height * width * 3), 'B')
        try:
            self.lock.acquire()
            oldtimeout = self.ser.timeout
            self.ser.setTimeout(.01)
            self.ser.write(chr(Scribbler.GET_IMAGE))
            size= width*height
            line = BufferedRead(self.ser, size, start = 0)
            #create the image from the YUV layer
            for i in range(height):
                for j in range(width):   
                    if j >= 3:
                        # go to the left for other values
                        #vy = -1; vu = -2; y1v = -1; y1u = -3; uy = -1; uv = -2; y2u = -1; y2v = -3
                        vy = -1; vu = -2; y1v = -1; y1u = -3; uy = -1; uv = -2; y2u = -1; y2v = -3
                    else:
                        # go to the right for other values
                        vy = 1; vu = 2; y1v = 3; y1u = 1; uy = 1; uv = 2; y2u = 3; y2v = 1
                    
                                       #   0123 0123 0123
                    if ((j % 4) == 0): #3 #2   VYUY VYUY VYUY
                        V = ord(line[i * width + j])
                        Y = ord(line[i * width + j + vy])
                        U = ord(line[i * width + j + vu])
                    elif ((j % 4) == 1): #0 #3
                        Y = ord(line[i * width + j])
                        V = ord(line[i * width + j + y1v])
                        U = ord(line[i * width + j + y1u])
                    elif ((j % 4) == 2): #1 #0
                        U = ord(line[i * width + j])
                        Y = ord(line[i * width + j + uy])
                        V = ord(line[i * width + j + uv])
                    elif ((j % 4) == 3): #2 #1
                        Y = ord(line[i * width + j])
                        U = ord(line[i * width + j + y2u])
                        V = ord(line[i * width + j + y2v])
                    U = U - 128
                    V = V - 128
                    Y = Y
                    buffer[(i * width + j) * 3 + 0] = max(min(Y + 1.13983 * V, 255), 0)
                    buffer[(i * width + j) * 3 + 1] = max(min(Y - 0.39466*U-0.58060*V, 255), 0)
                    buffer[(i * width + j) * 3 + 2] = max(min(Y + 2.03211*U, 255), 0)
            self.ser.setTimeout(oldtimeout)
        finally:
            self.lock.release()
            
        return buffer

    def _grab_array_bilinear_horizontal(self):
        width = 256
        height = 192
        buffer = array([0] * (height * width * 3), 'B')
        try:
            self.lock.acquire()
            oldtimeout = self.ser.timeout
            self.ser.setTimeout(.01)
            self.ser.write(chr(Scribbler.GET_IMAGE))
            size= width*height
            line = BufferedRead(self.ser, size, start = 0)
            #create the image from the YUV layer
            for i in range(height):
                for j in range(width):
                    vy = 1; vu = 2; y1v = 3; y1u = 1; uy = 1; uv = 2; y2u = 3; y2v = 1
                    vy2 = -1; vu2 = -2; y1v2 = -1; y1u2 = -3; uy2 = -1; uv2 = -2; y2u2 = -1; y2v2 = -3

                    if j >= 3:
                        # go to the left for other values
                        #vy = -1; vu = -2; y1v = -1; y1u = -3; uy = -1; uv = -2; y2u = -1; y2v = -3
                        vy = -1; vu = -2; y1v = -1; y1u = -3; uy = -1; uv = -2; y2u = -1; y2v = -3
                        if j < (width - 4):
                            vy2 = +3; vu2 = +2; y1v2 = +3; y1u2 = +1; uy2 = +3; uv2 = +2; y2u2 = +3; y2v2 = +1


                                #   0123 0123 0123
                    if ((j % 4) == 0): #3 #2   VYUY VYUY VYUY
                        V = ord(line[i * width + j])
                        Y = 0.5*ord(line[i * width + j + vy]) + 0.5*ord(line[(i) * width + j + vy2])
                        U = 0.5*ord(line[i * width + j + vu]) + 0.5*ord(line[(i) * width + j + vu2])
                    elif ((j % 4) == 1): #0 #3
                        Y = ord(line[i * width + j])
                        V = 0.5*ord(line[i * width + j + y1v]) + 0.5*ord(line[(i) * width + j + y1v2])
                        U = 0.5*ord(line[i * width + j + y1u]) + 0.5*ord(line[(i) * width + j + y1u2]) 
                    elif ((j % 4) == 2): #1 #0
                        U = ord(line[i * width + j])
                        Y = 0.5*ord(line[i * width + j + uy]) + 0.5*ord(line[(i) * width + j + uy2])
                        V = 0.5*ord(line[i * width + j + uv]) + 0.5*ord(line[(i) * width + j + uv2])
                    elif ((j % 4) == 3): #2 #1
                        Y = ord(line[i * width + j])
                        U = 0.5*ord(line[i * width + j + y2u]) + 0.5*ord(line[(i) * width + j + y2u2])
                        V = 0.5*ord(line[i * width + j + y2v]) + 0.5*ord(line[(i) * width + j + y2v2])
                    U = U - 128
                    V = V - 128
                    Y = Y
                    buffer[(i * width + j) * 3 + 0] = max(min(Y + 1.13983 * V, 255), 0)
                    buffer[(i * width + j) * 3 + 1] = max(min(Y - 0.39466*U-0.58060*V, 255), 0)
                    buffer[(i * width + j) * 3 + 2] = max(min(Y + 2.03211*U, 255), 0)
            self.ser.setTimeout(oldtimeout)
        finally:
            self.lock.release()
        return buffer

    def _grab_array_bilinear_vert(self):
        width = 256
        height = 192
        buffer = array([0] * (height * width * 3), 'B')
        try:
            self.lock.acquire()
            oldtimeout = self.ser.timeout
            self.ser.setTimeout(.01)
            self.ser.write(chr(Scribbler.GET_IMAGE))
            size= width*height
            line = BufferedRead(self.ser, size, start = 0)
            #create the image from the YUV layer
            for i in range(height):
                if i < 1:
                    n = 0
                else:
                    n = 1

                for j in range(width):   
                    if j >= 3:
                        # go to the left for other values
                        #vy = -1; vu = -2; y1v = -1; y1u = -3; uy = -1; uv = -2; y2u = -1; y2v = -3
                        vy = -1; vu = -2; y1v = -1; y1u = +1; uy = -1; uv = -2; y2u = -1;
                        if j < (width - 2):
                            y2v = +1
                        else:
                            y2v = -3
                    else:
                        # go to the right for other values
                        vy = 1; vu = 2; y1v = 3; y1u = 1; uy = 1; uv = 2; y2u = 3; y2v = 1

                            #   0123 0123 0123
                    if ((j % 4) == 0): #3 #2   VYUY VYUY VYUY
                        V = ord(line[i * width + j])
                        Y = 0.5*ord(line[i * width + j + vy]) + 0.5*ord(line[(i-n) * width + j + vy])
                        U = 0.5*ord(line[i * width + j + vu]) + 0.5*ord(line[(i-n) * width + j + vu])
                    elif ((j % 4) == 1): #0 #3
                        Y = ord(line[i * width + j])
                        V = 0.5*ord(line[i * width + j + y1v]) + 0.5*ord(line[(i-n) * width + j + y1v])
                        U = 0.5*ord(line[i * width + j + y1u]) + 0.5*ord(line[(i-n) * width + j + y1u]) 
                    elif ((j % 4) == 2): #1 #0
                        U = ord(line[i * width + j])
                        Y = 0.5*ord(line[i * width + j + uy]) + 0.5*ord(line[(i-n) * width + j + uy])
                        V = 0.5*ord(line[i * width + j + uv]) + 0.5*ord(line[(i-n) * width + j + uv])
                    elif ((j % 4) == 3): #2 #1
                        Y = ord(line[i * width + j])
                        U = 0.5*ord(line[i * width + j + y2u]) + 0.5*ord(line[(i-n) * width + j + y2u])
                        V = 0.5*ord(line[i * width + j + y2v]) + 0.5*ord(line[(i-n) * width + j + y2v])
                    U = U - 128
                    V = V - 128
                    Y = Y
                    buffer[(i * width + j) * 3 + 0] = max(min(Y + 1.13983 * V, 255), 0)
                    buffer[(i * width + j) * 3 + 1] = max(min(Y - 0.39466*U-0.58060*V, 255), 0)
                    buffer[(i * width + j) * 3 + 2] = max(min(Y + 2.03211*U, 255), 0)
            self.ser.setTimeout(oldtimeout)
        finally:
            self.lock.release()
        return buffer

    def getBattery(self):
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.GET_BATTERY))
            retval = read_2byte(self.ser) / 20.9813
        finally:
            self.lock.release()
        return retval
    
    def setBrightPower(self, power):
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_DONGLE_IR))
            self.ser.write(chr(power))
        finally:
            self.lock.release()

    def setLEDFront(self, value):
        value = int(min(max(value, 0), 1))
        try:
            self.lock.acquire()
            if isTrue(value):
                self.ser.write(chr(Scribbler.SET_DONGLE_LED_ON))
            else:
                self.ser.write(chr(Scribbler.SET_DONGLE_LED_OFF))
        finally:
            self.lock.release()

    def setLEDBack(self, value):
        if value > 1:
            value = 1
        elif value <= 0:
            value = 0
        else:
            value = int(float(value) * (255 - 170) + 170) # scale
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_DIMMER_LED))
            self.ser.write(chr(value))
        finally:
            self.lock.release()

    def getObstacle(self, value=None):
        if value == None:
            return self.get("obstacle")
        try:            
            self.lock.acquire()
            if value in ["left", 0]:
                self.ser.write(chr(Scribbler.GET_DONGLE_L_IR))
            elif value in ["middle", "center", 1]:
                self.ser.write(chr(Scribbler.GET_DONGLE_C_IR))
            elif value in ["right", 2]:
                self.ser.write(chr(Scribbler.GET_DONGLE_R_IR))
            retval = read_2byte(self.ser)
        finally:
            self.lock.release()
        return retval       

    def getBright(self, window=None):
        # left, middle, right

        # assumes this configuartion of the windows
        # conf_gray_window(self.ser, 0, 0, 0,    84, 191, 1, 1)
        # conf_gray_window(self.ser, 1, 84,  0, 170, 191, 1, 1)
        # conf_gray_window(self.ser, 2, 170, 0, 254, 191, 1, 1)

        if window == None or window == "all":
            return self.get("bright")
        if type(window) == str:
            if window in ["left"]:
                window = 0
            elif window in ["middle", "center"]:                
                window = 1
            elif window in ["right"]:
                window = 2
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.GET_WINDOW_LIGHT))
            self.ser.write(chr(window))
            retval = read_3byte(self.ser) #/ (63.0 * 192.0 * 255.0)
        finally:
            self.lock.release()
        return retval 

    def getBlob(self):
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.GET_BLOB))
            #self.ser.write(chr(window))
            numpixs = read_2byte(self.ser)
            xloc = ord(self.ser.read(1))
            yloc = ord(self.ser.read(1))
        finally:
            self.lock.release()
        return (numpixs, xloc, yloc)

    def setForwardness(self, direction):
        if direction in ["fluke-forward", 1]:
            direction = 1
        elif direction in ["scribbler-forward", 0]:
            direction = 0
        else:
            raise AttributeError("unknown direction: '%s': should be 'fluke-forward' or 'scribbler-forward'" % direction)
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_FORWARDNESS))
            self.ser.write(chr(direction))
        finally:
            self.lock.release()

    def setIRPower(self, power):
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_DONGLE_IR))
            self.ser.write(chr(power))
        finally:
            self.lock.release()

    def setWhiteBalance(self, value):
        try:
            self.lock.acquire()
            if isTrue(value):
                self.ser.write(chr(Scribbler.SET_WHITE_BALANCE))
            else:
                self.ser.write(chr(Scribbler.SET_NO_WHITE_BALANCE))
        finally:
            self.lock.release()
    
    def reboot(self):
        try:
            self.lock.acquire()
            self.ser.write(chr(Scribbler.SET_RESET_SCRIBBLER))
        finally:
            self.lock.release()

    def set_cam_param(self, addr, byte):
        try:
            self.lock.acquire()
            self.ser.write(chr(self.SET_CAM_PARAM))
            self.ser.write(chr(addr))
            self.ser.write(chr(byte))
            time.sleep(.15) # camera needs time to reconfigure
        finally:
            self.lock.release()
    
    def get_cam_param(self, addr):
        try:
            self.lock.acquire()
            self.ser.write(chr(self.GET_CAM_PARAM))
            self.ser.write(chr(addr))
            v = ord(self.ser.read(1)) 
        finally:
            self.lock.release()
        return v

    def darkenCamera(self, level=0):
        if self.debug:
            print "Turning off White Balance, Gain Control, and Exposure Control", level

            
        self.set_cam_param(self.CAM_COMA, self.CAM_COMA_WHITE_BALANCE_OFF)
        self.set_cam_param(self.CAM_COMB,
                           (self.CAM_COMB_GAIN_CONTROL_OFF & self.CAM_COMB_EXPOSURE_CONTROL_OFF))
        self.set_cam_param(0, level)
        self.set_cam_param(1, 0)
        self.set_cam_param(2, 0)
        self.set_cam_param(6, 0)    
        self.set_cam_param(0x10, 0)

    def manualCamera(self, gain=0x00, brightness=0x80, exposure=0x41):
        if self.debug:
            print "Turning off White Balance, Gain Control, and Exposure Control", level            
        self.set_cam_param(self.CAM_COMA, self.CAM_COMA_WHITE_BALANCE_OFF)
        self.set_cam_param(self.CAM_COMB,
                           (self.CAM_COMB_GAIN_CONTROL_OFF & self.CAM_COMB_EXPOSURE_CONTROL_OFF))
        self.set_cam_param(0x00, gain)
        self.set_cam_param(0x06, brightness)
        self.set_cam_param(0x10, exposure)

    def autoCamera(self):

        if self.debug:
            print "Turning on White Balance, Gain Control, and Exposure Control"

        self.set_cam_param(0, 0)
        self.set_cam_param(1, 0x80)
        self.set_cam_param(2, 0x80)
        self.set_cam_param(6, 0x80)
        self.set_cam_param(0x10, 0x41)
        self.set_cam_param(self.CAM_COMA, self.CAM_COMA_DEFAULT)
        self.set_cam_param(self.CAM_COMB, self.CAM_COMB_DEFAULT)

        
    ########################################################## End Dongle Commands

    def reset(self):
        for p in [127, 127, 127, 127, 0, 0, 0, 0]:
            self.setSingleData(p, 0)
        self.setName("Scribby")

    def setData(self, position, value):
        data = self._get(Scribbler.GET_DATA, 8)
        data[position] = value
        return self._set(*([Scribbler.SET_DATA] + data))

    def setSingleData(self,position,value):
        data = [position,value]
        return self._set(  *([Scribbler.SET_SINGLE_DATA] + data)  )

    def setEchoMode(self, value):
        if isTrue(value): self._set(Scribbler.SET_ECHO_MODE, 1)
        else:             self._set(Scribbler.SET_ECHO_MODE, 0)
        time.sleep(.25)
        self.ser.flushInput()
        self.ser.flushOutput()
        return

    def set(self, item, position, value = None):
        item = item.lower()
        if item == "led":
            if type(position) in [int, float]:
                if position == 0:
                    if isTrue(value): return self._set(Scribbler.SET_LED_LEFT_ON)
                    else:             return self._set(Scribbler.SET_LED_LEFT_OFF)
                elif position == 1:
                    if isTrue(value): return self._set(Scribbler.SET_LED_CENTER_ON)
                    else:             return self._set(Scribbler.SET_LED_CENTER_OFF)
                elif position == 2:
                    if isTrue(value): return self._set(Scribbler.SET_LED_RIGHT_ON)
                    else:             return self._set(Scribbler.SET_LED_RIGHT_OFF)
                else:
                    raise AttributeError("no such LED: '%s'" % position)
            else:
                position = position.lower()
                if position == "center":
                    if isTrue(value): return self._set(Scribbler.SET_LED_CENTER_ON)
                    else:             return self._set(Scribbler.SET_LED_CENTER_OFF)
                elif position == "left":
                    if isTrue(value): return self._set(Scribbler.SET_LED_LEFT_ON)
                    else:             return self._set(Scribbler.SET_LED_LEFT_OFF)
                elif position == "right":
                    if isTrue(value): return self._set(Scribbler.SET_LED_RIGHT_ON)
                    else:             return self._set(Scribbler.SET_LED_RIGHT_OFF)
                elif position == "front":
                    return self.setLEDFront(value)
                elif position == "back":
                    return self.setLEDBack(value)
                elif position == "all":
                    if isTrue(value): return self._set(Scribbler.SET_LED_ALL_ON)
                    else:             return self._set(Scribbler.SET_LED_ALL_OFF)
                else:
                    raise AttributeError("no such LED: '%s'" % position)
        elif item == "name":
            position = position + (" " * 16)
            name1 = position[:8].strip()
            name1_raw = map(lambda x:  ord(x), name1)
            name2 = position[8:16].strip()
            name2_raw = map(lambda x:  ord(x), name2)
            self._set(*([Scribbler.SET_NAME1] + name1_raw))
            self._set(*([Scribbler.SET_NAME2] + name2_raw))
        elif item == "password":
            position = position + (" " * 16)
            pass1 = position[:8].strip()
            pass1_raw = map(lambda x:  ord(x), pass1)
            pass2 = position[8:16].strip()
            pass2_raw = map(lambda x:  ord(x), pass2)
            self._set(*([Scribbler.SET_PASS1] + pass1_raw))
            self._set(*([Scribbler.SET_PASS2] + pass2_raw))
        elif item == "whitebalance":
            self.setWhiteBalance(position)
        elif item == "irpower":
            self.setIRPower(position)
        elif item == "volume":
            if isTrue(position):
                self._volume = 1
                return self._set(Scribbler.SET_LOUD)
            else:
                self._volume = 0
                return self._set(Scribbler.SET_QUIET)
        elif item == "startsong":
            self.startsong = position
        elif item == "echomode":
            return self.setEchoMode(position)
        elif item == "data":
            return self.setData(position, value)
        elif item == "password":
            return self.setPassword(position)
        elif item == "forwardness":
            return self.setForwardness(position)
        else:
            raise ("invalid set item name: '%s'" % item)
   
    # Sets the fudge values (in memory, and on the flash memory on the robot)
    def setFudge(self,f1,f2,f3,f4):

        self._fudge[0] = f1
        self._fudge[1] = f2
        self._fudge[2] = f3
        self._fudge[3] = f4

        # Save the fudge data (in integer 0..255 form) to the flash memory
        #f1-f4 are float values 0..2, convert to byte values
        # But to make things quick, only save the ones that have changed!
        # 0..255 and save.

        if self._oldFudge[0] != self._fudge[0] :
            if (self.dongle == None):
                self.setSingleData(0,  int(self._fudge[0] * 127.0) )
            else:
                self.setSingleData(3,  int(self._fudge[0] * 127.0) )

            self._oldFudge[0] = self._fudge[0] 

        if self._oldFudge[1] != self._fudge[1] :
            if (self.dongle == None):
                self.setSingleData(1,  int(self._fudge[1] * 127.0) )
            else:
                self.setSingleData(2,  int(self._fudge[1] * 127.0) )
            self._oldFudge[1] = self._fudge[1]


        if self._oldFudge[2] != self._fudge[2]:
            if (self.dongle == None):
                self.setSingleData(2,  int(self._fudge[2] * 127.0) )
            else:
                self.setSingleData(1,  int(self._fudge[2] * 127.0) )
            self._oldFudge[2] = self._fudge[2]
        
        if self._oldFudge[3] != self._fudge[3] :
            if (self.dongle == None):
                self.setSingleData(3,  int(self._fudge[3] * 127.0) )
            else:
                self.setSingleData(0,  int(self._fudge[3] * 127.0) )
            self._oldFudge[3] = self._fudge[3]
                
    
   #Called when robot is initialized, after serial connection is established.
   # Checks to see if the robot has fudge factors saved in it's data area
   # 0,1,2,3, and uses them. If the robot has zeros, it replaces them with 127
   # which is the equivalent of no fudge. Each factor goes from 0..255, where
   # a 127 is straight ahead (no fudging)
    def loadFudge(self):

        for i in range(4):
            self._fudge[i] = self.get("data",i)
            if self._fudge[i] == 0:
                self._fudge[i] = 127
            self._fudge[i] = self._fudge[i] / 127.0 # convert back to floating point!

        if (self.dongle != None):
            self._fudge[0], self._fudge[1], self._fudge[2], self._fudge[3] = self._fudge[3], self._fudge[2], self._fudge[1], self._fudge[0] 

    #Gets the fudge values (from memory, so we don't get penalized by a slow
    # serial link)
    def getFudge(self):
        return(self._fudge[0],self._fudge[1],self._fudge[2],self._fudge[3])

    def stop(self):
        self._lastTranslate = 0
        self._lastRotate = 0
        return self._set(Scribbler.SET_MOTORS_OFF)

    def hardStop(self):
        values = [Scribbler.SET_MOTORS_OFF]
        self._lastTranslate = 0
        self._lastRotate = 0
        self._write(values)
        self._read(Scribbler.PACKET_LENGTH) # read echo
        self._lastSensors = self._read(11) # single bit sensors


    def translate(self, amount):
        self._lastTranslate = amount
        self._adjustSpeed()

    def rotate(self, amount):
        self._lastRotate = amount
        self._adjustSpeed()

    def move(self, translate, rotate):
        self._lastTranslate = translate
        self._lastRotate = rotate
        self._adjustSpeed()

    def getLastSensors(self):
        retval = self._lastSensors
        return {"light": [retval[2] << 8 | retval[3], retval[4] << 8 | retval[5], retval[6] << 8 | retval[7]],
                "ir": [retval[0], retval[1]], "line": [retval[8], retval[9]], "stall": retval[10]}

    def update(self):
        pass

####################### Private

    def _adjustSpeed(self):
        left  = min(max(self._lastTranslate - self._lastRotate, -1), 1)
        right  = min(max(self._lastTranslate + self._lastRotate, -1), 1)


        
        # JWS additions for "calibration" of motors.
        # Use fudge values 1-4 to change the actual power given to each
        # motor based upon the forward speed.
        #
        # This code is here for documentation purposes only.
        # 
        # The algorithm shown here is now implemented on the basic stamp
        # on the scribbler directly.

        #fudge the left motor when going forward!
        #if (self._fudge[0] > 1.0 and left > 0.5 ):
           #left = left - (self._fudge[0] - 1.0)
        #if (self._fudge[1] > 1.0 and 0.5 >= left > 0.0):
           #left = left - (self._fudge[1] - 1.0)
        #fudge the right motor when going forward!
        #if (self._fudge[0] < 1.0 and right > 0.5):
          # right = right - (1.0 - self._fudge[0])
        #if (self._fudge[1] < 1.0 and 0.5 >= right > 0.0):
           #right = right - (1.0 - self._fudge[1])

        #Backwards travel is just like forwards travel, but reversed!
        #fudge the right motor when going backwards.
        #if (self._fudge[2] > 1.0 and 0.0 > right >= -0.5):
        #        right = right + (self._fudge[2] - 1.0)
        #if (self._fudge[3] > 1.0 and -0.5 > right ):
        #        right = right + (self._fudge[3] - 1.0)

        #fudge the left motor when going backwards.
        #if (self._fudge[2] < 1.0 and 0.0 > left >= -0.5):
        #        left = left + (1.0 - self._fudge[2]) 
        #if (self._fudge[3] < 1.0 and -0.5 > left):
        #        left = left + (1.0 - self._fudge[3])

  

	#print "actual power: (",left,",",right,")"
        
        #end JWS additions for "calibration of motors.
        leftPower = (left + 1.0) * 100.0
        rightPower = (right + 1.0) * 100.0
        
        self._set(Scribbler.SET_MOTORS, rightPower, leftPower)

    def _read(self, bytes = 1):
        
        if self.debug:
            print "Trying to read", bytes, "bytes", "timeout =", self.ser.timeout

        c = self.ser.read(bytes)
        
        if self.debug:
            print "Initially read", len(c), "bytes:",
            print map(lambda x:"0x%x" % ord(x), c)
            
	# sklar: this "bug fix" doesn't work in my configuration (01/01/09)
        # .nah. bug fix
        #while (bytes > 1 and len(c) < bytes):
        #    c = c + self.ser.read(bytes-len(c))
        #    if self.debug:
        #        print map(lambda x:"0x%x" % ord(x), c)
        # .nah. end bug fix

        if self.debug:
            print "_read (%d)" % len(c)
            print map(lambda x:"0x%x" % ord(x), c)

        if self.dongle == None:
            time.sleep(0.01) # HACK! THIS SEEMS TO NEED TO BE HERE!
        if bytes == 1:
            x = -1
            if (c != ""):
                x = ord(c)            
            elif self.debug:
                print "timeout!"
                return x
        else:
            return map(ord, c)

    def _write(self, rawdata):
        t = map(lambda x: chr(int(x)), rawdata)
        data = string.join(t, '') + (chr(0) * (Scribbler.PACKET_LENGTH - len(t)))[:9]
        if self.debug:
            print "_write:", data, len(data),
            print "data:",
            print map(lambda x:"0x%x" % ord(x), data)
        if self.dongle == None:
            time.sleep(0.01) # HACK! THIS SEEMS TO NEED TO BE HERE!
        self.ser.write(data)      # write packets

    def _set(self, *values):
        try:
            self.lock.acquire() #print "locked acquired"
            self._write(values)
            test = self._read(Scribbler.PACKET_LENGTH) # read echo
            self._lastSensors = self._read(11) # single bit sensors
            #self.ser.flushInput()
            if self.requestStop:
                self.requestStop = 0
                self.stop()
                self.lock.release()
                raise KeyboardInterrupt
        finally:
            self.lock.release()
    

    def _get(self, value, bytes = 1, mode = "byte"):
        try:
            self.lock.acquire()
            self._write([value])
            self._read(Scribbler.PACKET_LENGTH) # read the echo
            if mode == "byte":
                retval = self._read(bytes)
            elif mode == "word":
                retvalBytes = self._read(bytes)
                retval = []
                for p in range(0,len(retvalBytes),2):
                    retval.append(retvalBytes[p] << 8 | retvalBytes[p + 1])
            elif mode == "line": # until hit \n newline
                retval = self.ser.readline()
                if self.debug:
                    print "_get(line)", retval
            #self.ser.flushInput()            
        finally:
            self.lock.release()

        return retval

    def _set_speaker(self, frequency, duration):            
        self._write([Scribbler.SET_SPEAKER, 
                   duration >> 8,
                   duration % 256,
                   frequency >> 8,
                   frequency % 256])
        
    def _set_speaker_2(self, freq1, freq2, duration):
        self._write([Scribbler.SET_SPEAKER_2, 
                  duration >> 8,
                  duration % 256,
                  freq1 >> 8,
                  freq1 % 256,
                  freq2 >> 8,
                  freq2 % 256])
        
def cap(c):
    if (c > 255): 
        return 255
    if (c < 0):
        return 0

    return c

def conf_window(ser, window, X_LOW, Y_LOW, X_HIGH, Y_HIGH, X_STEP, Y_STEP):
    ser.write(chr(Scribbler.SET_WINDOW))
    ser.write(chr(window)) 
    ser.write(chr(X_LOW)) 
    ser.write(chr(Y_LOW)) 
    ser.write(chr(X_HIGH))
    ser.write(chr(Y_HIGH))
    ser.write(chr(X_STEP))
    ser.write(chr(Y_STEP))

def conf_gray_window(ser, window, lx, ly, ux, uy, xstep, ystep):
    # Y's are on odd pixels
    if (lx % 2)== 0:
        lx += 1
    if (xstep % 2) == 1:
        xstep += 1
    conf_window(ser, window, lx, ly, ux, uy, xstep, ystep)

def conf_gray_image(ser):
    # skip every other pixel
    conf_window(ser, 0, 1, 0, 255, 191, 2, 2)
    
def grab_rle_on(ser):
    """
    Returns a list of pixels that match.
    """
    print "RLE"
    width = 256
    height = 192    
    blobs = zeros(((height + 1), (width + 1)), dtype=uint8)
    on_pxs = []
    line = ''
    ser.write(chr(Scribbler.GET_RLE))
    size=ord(ser.read(1))
    size = (size << 8) | ord(ser.read(1))
    if self.debug:
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
                if (inside):
                    on_pxs += [[j+z, i]]
            counter -= 1
    return on_pxs

def read_2byte(ser):
    hbyte = ord(ser.read(1))
    lbyte = ord(ser.read(1))
    lbyte = (hbyte << 8) | lbyte
    return lbyte

def read_3byte(ser):
    hbyte = ord(ser.read(1))
    mbyte = ord(ser.read(1))
    lbyte = ord(ser.read(1))
    lbyte = (hbyte << 16)| (mbyte << 8) | lbyte
    return lbyte

def write_2byte(ser, value):
    ser.write(chr((value >> 8) & 0xFF))
    ser.write(chr(value & 0xFF))

def read_mem(ser, page, offset):
    ser.write(chr(Scribbler.GET_SERIAL_MEM))
    write_2byte(ser, page)
    write_2byte(ser, offset)
    return ord(ser.read(1))

def write_mem(ser, page, offset, byte):
    ser.write(chr(Scribbler.SET_SERIAL_MEM))
    write_2byte(ser, page)
    write_2byte(ser, offset)
    ser.write(chr(byte))

def erase_mem(ser, page):
    ser.write(chr(Scribbler.SET_SERIAL_ERASE))
    write_2byte(ser, page)

# Also copied into system.py:
def set_scribbler_memory(ser, offset, byte):
    ser.write(chr(Scribbler.SET_SCRIB_PROGRAM))
    write_2byte(ser, offset)
    ser.write(chr(byte))
    
def set_scribbler_start_program(ser, size):
    ser.write(chr(Scribbler.SET_START_PROGRAM))
    write_2byte(ser, size)
            
def get_window_avg(ser, window):
    ser.write(chr(Scribbler.GET_WINDOW_LIGHT))
    ser.write(chr(window))
    return read_2byte(ser)

def quadrupleSize(line, width):
    retval = [" "] * len(line) * 4
    col = 0
    row = 0
    for c in line:
        retval[row       * 2 * width + col]   = c
        retval[row       * 2 * width + col+1] = c
        retval[(row + 1) * 2 * width + col]   = c
        retval[(row + 1) * 2 * width + col+1] = c
        col += 2
        if col == width * 2:
            col = 0
            row += 2
    return "".join(retval)

def set_ir_power(ser, power):
    ser.write(chr(Scribbler.SET_DONGLE_IR))
    ser.write(chr(power))


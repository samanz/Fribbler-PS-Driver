# Gaurav

from dongle import *
from intelhex import IntelHex
import sys
import time

#define UF_SUCCESS 42
#define UF_ERROR 1
#define UF_SEGMENT_SIZE 132

binarray = []

def uf_sendPage(page,binarray):
	global s
	segment = 0

	while segment < 264/132 :
		i = 0
		sum = 0
		for i in range (0,132) :
			s.write(chr(binarray[page*264 + segment*132 + i]))
			sum = sum + binarray[page*264 + segment*132 + i]
		s.write(chr(sum % 256))
		retval = ord(s.read(1))
		#print "Sum: %d Return: %d" % (sum % 256,retval)
		if retval == 42 :
			segment = segment + 1


def uf_recvPage(page,binarray):
	global s
	segment = 0

	while segment < 264/132 :
		i = 0
		sum = 0
		chksum = 0
		for i in range (0,132) :
			recv = ord(s.read(1))
			binarray[page*264 + segment*132 + i] = recv 
			sum = sum + recv 
		chksum = ord(s.read(1))
		#print "My sum: %d Recd chksum: %d" % (sum % 256,chksum)
		if chksum == sum % 256 :
			segment = segment + 1
			s.write(chr(42))
 		else :
			s.write(chr(1))


def uf_saveEEPROMdump():
	global eepromdump
	for i in range (0,512) :
		print '\r' + "%d %%" % ((i*100)/512),
		sys.stdout.flush()
		uf_recvPage(i,eepromdump)
	print ""

def uf_restoreEEPROMdump():
	global eepromdump
	for i in range (0,512) :
		print '\r' + "%d %%" % ((i*100)/512),
		sys.stdout.flush()
		uf_sendPage(i,eepromdump)
	print ""

def uf_storeinEEPROM():
	global s
	global arlen
	global binarray
	segs = arlen / 264
	if segs*264 < arlen :
		segs = segs + 1
		for i in range (arlen,segs*264):
			binarray.append(0)

	#print "Writing %d segments" % segs
	write_2byte(s,segs)
	for i in range (0,segs) :
		print '\r' + "%d %%" % ((i*100)/segs),
		sys.stdout.flush()
		uf_sendPage(i,binarray)
	print ""


if len(sys.argv) < 2:
	print sys.argv[0], "firmware-file-as-hex"
	sys.exit(-1)

ih = IntelHex(sys.argv[1])
binarray = ih.tobinarray()
arlen = len(binarray)	
print "%d bytes of firmware." % arlen

#print "checksumming interrupt vectors"
for i in range(20,24):
	binarray[i] = 0
sum=0
for i in range(0,8):
	temp_int = 0
	temp_int = binarray[i*4 + 0] | binarray[i*4 + 1] << 8 | binarray[i*4 + 2] << 16 | binarray[i*4 + 3] << 24
	sum = sum + temp_int

sum = -sum

binarray[20] = sum & 0x000000ff
binarray[21] = (sum >> 8) & 0x000000ff
binarray[22] = (sum >> 16) & 0x000000ff
binarray[23] = (sum >> 24) & 0x000000ff 
	
sum=0
for i in range(0,8):
	temp_int = 0
	temp_int = binarray[i*4 + 0] | binarray[i*4 + 1] << 8 | binarray[i*4 + 2] << 16 | binarray[i*4 + 3] << 24
	sum = sum + temp_int


for i in range(0,arlen):
	if i % 8192 == 0 :
		sum = 0
	sum = sum + binarray[i]


#declare a finite sized array to hold eeprom dump. 
#Dynamic appending of lists always comes with a performance hit
eepromdump = [0] * 135168

s = serial.Serial("/dev/tty.scribbler", timeout=20, rtscts=1)

try:
    s.flushOutput()
    s.flushInput()
    #print "Getting old EEPROM"
    #s.write(chr(SAVE_EEPROM))
    #uf_saveEEPROMdump()
    print "Sending firmware"
    s.write(chr(UPDATE_FIRMWARE))
    uf_storeinEEPROM()
    print "Waiting for reboot"
    time.sleep(2)
    
finally:
    s.close()


from myro import *

initialize("/dev/tty.scribbler")

GO=True

TOO_LEFT = 96.0
TOO_RIGHT = 160.0
GAIN = 0.4
FORWARD = GAIN


print "select the color to track"
#show(takePicture())
#wait(5)
print "starting loop"

lastdir = 'r'

for i in range(40):

    print i

    b = currentTime()
    blob = takePicture("blob")
    
    avg_x = 0.0
    px_cnt = 0
    for px in getPixels(blob):
        if getRed(px) > 0:
            avg_x += getX(px)
            px_cnt +=1

    if px_cnt > 10:
        avg_x /= px_cnt

    a = currentTime()
    print "Time = ", (a-b)*1000.0
 
    show(blob)
    print "avg x = ", avg_x
    
    if (px_cnt < 10):
        if GO:
            if lastdir == 'l':
                turnLeft(GAIN)
            else:
                turnRight(GAIN)
            print "searching"
    elif (avg_x < TOO_LEFT):
        lastdir = 'l'
        if GO:
            turnLeft(GAIN)
        print "turning left"
    elif (avg_x > TOO_RIGHT):
        lastdir = 'r'
        if GO:
            turnRight(GAIN)
        print "turning right"
    else:
        print "headed toward the object"
        if GO:
            forward(FORWARD)

stop()

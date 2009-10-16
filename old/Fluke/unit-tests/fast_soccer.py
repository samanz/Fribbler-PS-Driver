from myro import *


init()

GO=True

TOO_LEFT = 80.0
TOO_RIGHT = 144.0
GAIN = 0.45

show(takePicture())
print "select the color to track"
#wait(2)
show(takePicture('blob'))

lastdir = 'r'

while True:

    px_cnt, avg_x, avg_y = getBlob()

    
    if (px_cnt < 20):
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
            forward(GAIN)

stop()

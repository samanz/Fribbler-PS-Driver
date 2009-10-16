from myro import *

init()

GO=True
TOO_LEFT = 40.0
TOO_RIGHT = 230.0
GAIN = 0.45
lastdir = 'r'

darkenCamera(255)
configureBlob(y_low=18, y_high = 255, u_low=0, u_high = 128, v_low=128, v_high = 255, smooth_thresh=1)

for i in range(9):

    px_cnt, avg_x, avg_y = getBlob()
    
    print px_cnt, avg_x
    
    if (px_cnt == 0):
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

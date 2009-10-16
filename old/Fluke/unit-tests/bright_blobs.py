from myro import *

init()

darkenCamera()
configureBlob(y_low=100, y_high = 255)

for i in range(10):
    p = takePicture('blob')
    print getBlob()
    show(p)

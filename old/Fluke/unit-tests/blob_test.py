from myro import *


init()

print "Select your blob box"
show(takePicture())
wait(5)

num = 200
rle_avg = 0.0
blob_avg = 0.0

zeroframes = 0
for i in range(num):
    b = currentTime()
    p = takePicture("blob")
    a = currentTime()
    show(p)
    print "RLE", (a - b) * 1000.0
    
    rle_avg += (a-b)
    
    b = currentTime()
    pxs, x, y =  getBlob()
    a = currentTime()
    print "blob count", (a - b) * 1000.0, pxs, x, y
    blob_avg += (a-b)
    
    if pxs < 100:
        zeroframes +=  1

print "RLE/blob avg", rle_avg/num
print "getBlob() avg", blob_avg/num
print "zero frames", zeroframes

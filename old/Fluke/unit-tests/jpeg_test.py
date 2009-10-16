from myro import *

initialize()

avg = 0.0
num = 25

for i in range(num):
    b = currentTime()
    p = takePicture('jpeg')
    a = currentTime()
    show(p)
    #name = "img-%02d.jpg" % i
    #savePicture(p, name)
    print (a - b) * 1000.0
    avg += (a-b)

print avg / num
    

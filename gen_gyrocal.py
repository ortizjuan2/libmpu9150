import numpy as np
from sys import exit

try:
    f = open('salida.txt', 'r')
except IOError as e:
    print("data file not found!")
    exit()

data = {}
gx = []
gy = []
gz = []

for line in f:
    l = line.split(' ')
    gx.append(float(l[3]))
    gy.append(float(l[4]))
    gz.append(float(l[5]))

f.close()

gx_offset = (min(gx)+max(gx))/2.0
gy_offset = (min(gy)+max(gy))/2.0
gz_offset = (min(gz)+max(gz))/2.0

f = open('gyrocal.txt', 'w')
f.write(str(gx_offset) + " " + str(gy_offset) + " " + str(gz_offset) + "\n")
f.close()

print('gyrocal.txt generated successfully!')



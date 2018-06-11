import numpy as np 
import matplotlib.pyplot as plt 


B = 30.6194   # Magnetic field stregth in uT
I = 0.4881104 # Magnetic Inclination angle in RAD

#RX = np.array([[1,0,0],[0, np.cos(np.pi), -np.sin(np.pi)],[0, np.sin(np.pi), np.cos(np.pi)]])
RX = np.array([[1,0,0],[0, -1, 0],[0, 0, -1]])
# Rotate around z by 90deg
theta = 1.5707963267948966
#RZ = np.array([[np.cos(theta), -np.sin(theta), 0],
#                [np.sin(theta), np.cos(theta), 0],
#                [0, 0, 1]])
RZ = np.array([[0, -1, 0], [1, 0, 0], [0, 0 , 1]])
## combine the two rotations
RXZ = np.dot(RZ, RX)

data_file_name = "../salida/salida_090618-1529.txt"
data_file_name_inclined = "../salida/salida_090618-1536-inclinado.txt"

f = open(data_file_name_inclined, 'r')

accel_data = []
gyro_data = []
compass_data = []

for line in f:
    l = line.split(" ")
    accel_data.append([float(l[0]), float(l[1]), float(l[2])])
    gyro_data.append([float(l[3]), float(l[4]), float(l[5])])
    compass_data.append([float(l[6]), float(l[7]), float(l[8])])

f.close()

accel = np.array(accel_data)
gyro = np.array(gyro_data)
compass = np.array(compass_data)

pitch = []
roll = []
yaw = []

for i in range(accel.shape[0]):
    v = np.dot(RXZ, accel[i])
    p = np.arcsin(-v[0] / 9.8)
    r = np.arctan2(v[1], v[2])
    y = np.arctan2(np.cos(p) * (compass[i,2] * np.sin(r) - compass[i,1] * np.cos(r)), 
                    (compass[i,0] + B * np.sin(I) * np.sin(p)))
    pitch.append(p)
    roll.append(r)
    yaw.append(y)


## to compute the correct heading
## it is necessary to rotate accelerometer readings
## 180deg around x and then 90 degres around z
## to aling the accelerometer with the compass axis
v = np.dot(RXZ, accel[0])
v = np.dot(RZ, v)
p = np.arcsin(-v[0] / 9.8)
r = np.arctan2(v[1], v[2])
y = np.arctan2(np.cos(p) * (compass[0,2] * np.sin(r) - compass[0,1] * np.cos(r)), 
                    (compass[0,0] + B * np.sin(I) * np.sin(p)))





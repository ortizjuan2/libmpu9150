import numpy as np 
from numpy import cos, sin, pi as PI
# import matplotlib.pyplot as plt 
from plotly.offline import plot
from plotly.graph_objs import Scatter, Figure, Layout, XAxis, YAxis
from plotly import tools


B = 30.6194   # Magnetic field stregth in uT
I = 0.4881104 # Magnetic Inclination angle in RAD

ACCEL_OFF = np.array([-0.016258333333, -0.001021, 0.0390545])
COMPASS_HARD_IRON = np.array([7.6640600000000001, 0.12889999999999979, 0.0])
COMPASS_SOFT_IRON = np.array([1.0, 1.0, 1.0])


def rotX(v, theta):
    R = np.array([[1, 0, 0], [0, cos(theta), -sin(theta)], [0, sin(theta), cos(theta)]])
    return np.dot(R, v)

def rotY(v, theta):
    R = np.array([[cos(theta), 0, sin(theta)], [0, 1, 0], [-sin(theta), 0, cos(theta)]])
    return np.dot(R, v)

def rotZ(v, theta):
    R = np.array([[cos(theta), -sin(theta), 0], [sin(theta), cos(theta), 0], [0, 0, 1]])
    return np.dot(R, v)

def norm(v):
    return v / np.sqrt(sum(v**2))

def deg(angle):
    return angle * 180. / PI

# Rotate compass vector 180 deg around X axis, and
# 90 deg around Z axis to align with accelerometer.
RXZ = np.array([[0, 1, 0], [1, 0, 0], [0, 0, -1]])

# data_file_name = "../salida/JUN17/salida_compass.txt"
# data_file_name_inclined = "../salida/salida_090618-1536-inclinado.txt"

# f = open(data_file_name, 'r')

# accel_data = []
# gyro_data = []
# compass_data = []

# for line in f:
#     l = line.split(" ")
#     accel_data.append([float(l[0]), float(l[1]), float(l[2])])
#     gyro_data.append([float(l[3]), float(l[4]), float(l[5])])
#     compass_data.append([float(l[6]), float(l[7]), float(l[8])])

# f.close()

# accel = np.array(accel_data)
# gyro = np.array(gyro_data)
# compass = np.array(compass_data)



# yaw = np.array([], dtype=np.float32)

# for i in range(accel.shape[0]):
#     ac = accel[i] - ACCEL_OFF
#     cp = compass[i]
#     # Rotate compass
#     cp = np.dot(RYZ, cp)
#     pitch = np.arcsin(-ac[0] / 9.8)
#     roll = np.arctan2(ac[1], ac[2])
#     # remove tilt
#     cp = rotX(cp, -roll)
#     cp = rotY(cp, -pitch)
#     # remove Hard and Soft Iron
#     cp = (cp - COMPASS_HARD_IRON) / COMPASS_SOFT_IRON
#     yw = np.arctan2(cos(pitch) * (cp[2] * sin(roll) - cp[1] * cos(roll)), 
#                     (cp[0] + B * sin(I) * sin(pitch)))
#     yaw = np.append(yaw, yw)

# # plt.plot(yaw, '-b', label="Yaw angle")
# # plt.legend()
# # plt.show()
# yaw_avg = sum(yaw)/len(yaw) * 180 / PI
# print ("yaw avg: %f") % yaw_avg

## ---
compass_file = "/home/jom/Documents/datasheets/MPU9150/salida/JUN17/salida_compass.txt"
f = open(compass_file, 'r')


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

# remove offset
accel = accel - ACCEL_OFF
# rotate compass
compassRotated = np.array([], dtype=np.float32)
for i in range(compass.shape[0]):
    ac = norm(accel[i])
    cp = compass[i]
    # rotate compass
    cp = np.dot(RXZ, cp)
    # pitch = np.arcsin(-ac[0] / 9.8)
    # roll = np.arctan2(ac[1], ac[2])
    # # Remove bank and elevation angles
    # cp = rotX(cp, -roll)
    # cp = rotY(cp, -pitch)
    compassRotated = np.append(compassRotated, cp)

compassRotated = compassRotated.reshape(compass.shape)

# find offset from center of ellipse
alfa = (compassRotated[:,0].max() + compassRotated[:,0].min()) / 2.0
beta = (compassRotated[:,1].max() + compassRotated[:,1].min()) / 2.0
# remove hard iron distortion
compassRotated = compassRotated - np.array([alfa, beta, 0.0])
# plot compass data

layout = Layout(
    xaxis=XAxis(
       range=[-15, 15],
    #    showgrid=True,
    #    zeroline=True,
    #    showline=True,
    #    gridcolor='#bdbdbd',
    #    gridwidth=2,
    #    zerolinecolor='#969696',
    #    zerolinewidth=2,
    #    linecolor='#636363',
    #    linewidth=2
    ),
    yaxis=YAxis(
        range=[-15,15],
        # showgrid=True,
        # zeroline=True,
        # showline=True,
        # gridcolor='#bdbdbd',
        # gridwidth=2,
        # zerolinecolor='#969696',
        # zerolinewidth=2,
        # linecolor='#636363',
        # linewidth=2
   ),
   height=900,
   width=900,
)

datacmp = Scatter(x=compassRotated[:,0], y=compassRotated[:,1], name='Compass Data', mode='markers',
            marker=dict(color='rgb(17, 157, 250)', size=6))
figure = Figure(data=[datacmp], layout=layout)
plot(figure)

# angle test
compass_file = "/home/jom/Documents/datasheets/MPU9150/salida/JUN20/salida.txt"
f = open(compass_file, 'r')

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

# remove offset
accel = accel - ACCEL_OFF


yaw = np.array([], dtype=np.float32)

for i in range(accel.shape[0]):
    ac = norm(accel[i])
    cp = compass[i]
    # Rotate compass
    cp = np.dot(RXZ, cp)
    pitch = np.arcsin(-ac[0] / 9.8)
    roll = np.arctan2(ac[1], ac[2])
    # remove tilt
    # cp = rotX(cp, -roll)
    # cp = rotY(cp, -pitch)
    # remove Hard and Soft Iron
    cp = (cp - COMPASS_HARD_IRON) / COMPASS_SOFT_IRON
    yw = np.arctan2(cos(pitch) * (cp[2] * sin(roll) - cp[1] * cos(roll)), 
                    (cp[0] + B * sin(I) * sin(pitch)))
    yaw = np.append(yaw, yw)

print("Yaw angle:{0:3.4f} ".format(deg(yaw.mean())))
#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#include "mpu9150.h"

using namespace std;

struct timespec req = {0};

int main(int argc, char * argv[]){
	int fd, fdm;
	fstream outfile("salida.txt", ios::out);
	outfile.exceptions(ios::eofbit | ios::badbit | ios::failbit);

	if(!outfile.is_open()){
		cout << "Error opening output file" << endl;
		return 0;
	}

	int milisec = 1000;
	//struct timespec req = {0};
	req.tv_sec = milisec / 1000;
	req.tv_nsec = (milisec % 1000) * 1000000L;

	MPU9150 mpu;
	if(mpu.init_sensor(0x68) == -1) {
		cout << "Error initializing sensor!" << endl;
		return -1;
	}
	// run for one hour static
	for(int i = 0; i < 3600; i++) {
		// wait 1 s
		nanosleep(&req, (struct timespec *)NULL);
		mpu.read_accel();
		mpu.read_gyro();
		mpu.read_compass();
		cout << "Run: " << i+1 << " "
		     << "accel: " << mpu.accel[0] << " " << mpu.accel[1] << " " << mpu.accel[2] << " "
		     << "gyro: " << mpu.gyro[0] << " " << mpu.gyro[1] << " " << mpu.gyro[2] << " "
		     << "compass: " << mpu.compass[0] << " " << mpu.compass[1] << " " << mpu.compass[2] << endl;
			 try{
			 	outfile << mpu.accel[0] << " " << mpu.accel[1] << " " << mpu.accel[2] << " ";
				 outfile << mpu.gyro[0] << " " << mpu.gyro[1] << " " << mpu.gyro[2] << " ";
				 outfile << mpu.compass[0] << " " << mpu.compass[1] << " " << mpu.compass[2] << endl;
			 }
			 catch(exception &e){
				 cout << e.what() << endl;
			 }
		// outfile << mpu.accel[0] << " "
		//         << mpu.accel[1] << " "
		//         << mpu.accel[2] << " "
		//         << mpu.gyro[0] << " "
		//         << mpu.gyro[1] << " "
		//         << mpu.gyro[2] << " "
		//         << mpu.compass[0] << " "
		//         << mpu.compass[1] << " "
		//         << mpu.compass[2] << endl;
	}
	outfile.close();
	return 0;

}

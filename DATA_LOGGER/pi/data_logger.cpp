#include <iostream>
#include <fstream> // for file
#include <cstdio> // for sprintf
#include <unistd.h> // for sleep

#include <wiringPiSPI.h>
#include <unistd.h>

/**********************
 *         pi  stm32
 * mosi    19   PC1
 * miso    21   pc2
 * sclk    23   pb10
 * ce1     11   pb12
 *********************/
 //compile with    g++ data_logger.cpp -l wiringPi -o data_logger

using namespace std;

// SPI chip select 1 on pi, and clock speed
#define SPI_CHANNEL 1
#define SPI_CLK_SPEED 1000000 //can be 500kHz to 32MHz


int main() {
	
	// setting up the file
	bool isUSB = 0;
	fstream meta_file;
	
	// If usb is not mounted, write to local
	meta_file.open("/media/feb/DATALOGGER/meta.txt", ios::in);
	if (!meta_file) {
		cout << "USB not Found! Write to local.\n";
		meta_file.open("/home/feb/Desktop/Data_logger/data/meta.txt", ios::in);
		if (!meta_file) {
			cout << "Error! Cannot read the local meta file.\n";
			return 0;
		}
	} else {
		cout << "USB Found!\n";
		isUSB = 1;
	}
	
	// read the number of runs
	int num_run;
	meta_file >> num_run;
	num_run++;
	cout << "Starting the " << num_run << "(th) run.\n";
	meta_file.close();
	
	//update the meta file with current run
	if(isUSB) {
		meta_file.open("/media/feb/DATALOGGER/meta.txt", ios::out | ios::trunc);
		if (!meta_file) {
			cout << "Error! Cannot write to the USB meta file.\n";
			return 0;
		}
	} else {
		meta_file.open("/home/feb/Desktop/Data_logger/data/meta.txt", ios::out | ios::trunc);
		if (!meta_file) {
			cout << "Error! Cannot write to the local meta file.\n";
			return 0;
		}
	}
	meta_file << num_run;
	meta_file.close();
	
	// create the log file for the current run
	char log_file_path[128];
	fstream log_file;
	if(isUSB) {
		sprintf(log_file_path, "/media/feb/DATALOGGER/%d.csv", num_run);
		log_file.open(log_file_path, ios::out);
		if (!log_file) {
			cout << "Error! Cannot write to the USB log file.\n";
			return 0;
		}
	} else {
		sprintf(log_file_path, "/home/feb/Desktop/Data_logger/data/%d.csv", num_run);
		log_file.open(log_file_path, ios::out);
		if (!log_file) {
			cout << "Error! Cannot write to the USB log file.\n";
			return 0;
		}
	}
	log_file.close();
	
	
	// setting up spi
	int fd;
	unsigned char buffer[128];

	cout << "Initializing" << endl ;

	fd = wiringPiSPISetup(SPI_CHANNEL, SPI_CLK_SPEED);
	if (fd == -1) {
	  cout << "Fail to Init SPI Communication!";
	  return 0;
	}

   cout << "Init result: " << fd << endl;
   usleep(10000); //wait for 10ms
	
	
	while(1) {
		log_file.open(log_file_path, ios::out | ios::app);
		read(fd, buffer, 128);
		
		if (buffer[0] != 10) {
			for(int i = 0; i < 128; i++) {
				if(buffer[i] == 10){
				   log_file << endl;
				   cout << endl;
				   break;
				}
				log_file << buffer[i];
				cout << buffer[i];
			}
		}
		  
		log_file.close();
		usleep(1000); //wait for 1ms
	}
	
	return 0;
}


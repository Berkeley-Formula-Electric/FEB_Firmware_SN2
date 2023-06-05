#include <iostream>
#include <fstream> // for file
#include <cstdio> // for sprintf
#include <cstring> // for memset
#include <unistd.h> // for sleep

#include <wiringPiSPI.h>
#include "sio_client.h"

#include <chrono>


/**********************
 *         pi  stm32
 * mosi    19   PC1
 * miso    21   pc2
 * sclk    23   pb10
 * ce0     24   pb12
 *********************/

using namespace std;
using namespace std::chrono;

// SPI chip select 0 on pi, and clock speed
#define SPI_CHANNEL 0
#define SPI_CLK_SPEED 500000 //can be 500kHz to 32MHz

int main() {

	sio::client h;
	h.connect("http://localhost:3000");


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
	int data_buffer_len = 50;
	char data_buffer[data_buffer_len];
	char c;

	cout << "Initializing" << endl ;

	fd = wiringPiSPISetup(SPI_CHANNEL, SPI_CLK_SPEED);
	if (fd == -1) {
	  cout << "Fail to Init SPI Communication!";
	  return 0;
	}

   cout << "Init result: " << fd << endl;
   usleep(10000); //wait for 10ms
   log_file.open(log_file_path, ios::out | ios::app);
   
   char* cptr;

	while(1) {
		auto start = high_resolution_clock::now();

		do {
			read(fd, &c, 1);
		}while(c != 60);

		for (int i = 0; i < data_buffer_len; i++) {
			read(fd, &c, 1);
			if(c == 62) {
				break;
			}
			data_buffer[i] = c;
		}
		cout << data_buffer << endl;
		log_file << data_buffer << endl;
		
		cptr = strtok(data_buffer, ",");
		int count = 0;
		sio::message::list data_list;
		// parse the string into <sender>, <message_type>, <data>
		while (cptr) {
			// limit only few sender msg to driver UI
			if (count == 1) {
				if(strcmp(cptr, "BMS")!=0 && strcmp(cptr, "RMS_INFO")!=0 && strcmp(cptr, "SW")!=0) {
					break;
				}
			}
			// the first entry of the string is time_stamp. not sending to driver UI
			if(count >= 1) {
				data_list.push(sio::string_message::create(cptr));
			}
			count++;
			cptr = strtok(NULL, ",");
		}
		memset(data_buffer, 0, data_buffer_len);
		
		if (count != 4) {
			continue;
		}
	
		h.socket()->emit("cpp_data", data_list);


		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop-start);
		cout << "Time: " << duration.count() << " us" << endl;
	}

	log_file.close();


	return 0;
}


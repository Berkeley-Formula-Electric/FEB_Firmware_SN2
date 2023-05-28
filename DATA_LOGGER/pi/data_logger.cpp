#include <iostream>
#include <fstream> // for file
#include <cstdio> // for sprintf
#include <unistd.h> // for sleep

#include <wiringPiSPI.h>
#include<FEB_CAN_NODE.h> 

//Libraries for https communication with node js server
//#include <json/json.h> //May be necessary later but for now we can use strings
#include <curl/curl.h>

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

//Forward declarations 
void updateData(string updateString, CURL* curl);

int main() {

	//set up JSON string May be used in the future if JSON package is added
	//Json::Value root; 
	//Json::Reader reader; 
	
	//INITIALIZE CURL PARAMETERS
	CURL *curl;
	struct cur_slist *headers = NULL; 
	curl_global_init(CURL_GLOBAL_ALL); 
	curl = curl_easy_init();
	if(curl){
		//set URL and request type 
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/api/postdata");
		curl_easy_setopt(curl, CURLOPT_POST,1L);

		//set request headers
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}


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

		//Sending POST Request to Node Js server
		//TODO: Not sure how the SPI_MESSAGE works so the following will need some edits
		string data = "param1" + to_string(SPI_MESSAGE.bits); 
		updateData(data, curl);

		log_file.close();
		usleep(1000); //wait for 1ms
	}

	//Free all curl related objects
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	curl_global_cleanup();
	
	return 0;
} 

void updateData(string updateString,CURL* curl) {
	CURLcode res;
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,updateString.c_str());
	res = curl_easy_perform(curl);
	if(res != CURLE_OK){
		cerr << "Error" << endl;
	}
}


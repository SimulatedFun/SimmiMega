#define TFT_CS   5//   // 3 goes to TFT CS
#define TFT_DC   27//   // 4 goes to TFT DC
#define TFT_MOSI 23  // 5 goes to TFT MOSI
#define TFT_SCK 18//  // 6 goes to TFT SCK/CLK
#define TFT_RESET  33//  // ESP RST to TFT RESET
#define TFT_MISO  19   // Not connected
//

#include <Arduino_GFX_Library.h>
Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);

#include <SPI.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS   15
static const int spiClk = 240000000; // 1 MHz
SPIClass hspi = SPIClass(HSPI);


#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <SPI.h>
#include <EEPROM_SPI_WE.h>


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
	Serial.printf("Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if(!root){
		Serial.println("Failed to open directory");
		return;
	}
	if(!root.isDirectory()){
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if(levels){
				listDir(fs, file.path(), levels -1);
			}
		} else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void createDir(fs::FS &fs, const char * path){
	Serial.printf("Creating Dir: %s\n", path);
	if(fs.mkdir(path)){
		Serial.println("Dir created");
	} else {
		Serial.println("mkdir failed");
	}
}

void removeDir(fs::FS &fs, const char * path){
	Serial.printf("Removing Dir: %s\n", path);
	if(fs.rmdir(path)){
		Serial.println("Dir removed");
	} else {
		Serial.println("rmdir failed");
	}
}

void readFile(fs::FS &fs, const char * path){
	Serial.printf("Reading file: %s\n", path);

	File file = fs.open(path);
	if(!file){
		Serial.println("Failed to open file for reading");
		return;
	}

	Serial.print("Read from file: ");
	while(file.available()){
		Serial.write(file.read());
	}
	file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
	Serial.printf("Writing file: %s\n", path);

	File file = fs.open(path, FILE_WRITE);
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
	if(file.print(message)){
		Serial.println("File written");
	} else {
		Serial.println("Write failed");
	}
	file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
	Serial.printf("Appending to file: %s\n", path);

	File file = fs.open(path, FILE_APPEND);
	if(!file){
		Serial.println("Failed to open file for appending");
		return;
	}
	if(file.print(message)){
		Serial.println("Message appended");
	} else {
		Serial.println("Append failed");
	}
	file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
	Serial.printf("Renaming file %s to %s\n", path1, path2);
	if (fs.rename(path1, path2)) {
		Serial.println("File renamed");
	} else {
		Serial.println("Rename failed");
	}
}

void deleteFile(fs::FS &fs, const char * path){
	Serial.printf("Deleting file: %s\n", path);
	if(fs.remove(path)){
		Serial.println("File deleted");
	} else {
		Serial.println("Delete failed");
	}
}

void testFileIO(fs::FS &fs, const char * path){
	File file = fs.open(path);
	static uint8_t buf[512];
	size_t len = 0;
	uint32_t start = millis();
	uint32_t end = start;
	if(file){
		len = file.size();
		size_t flen = len;
		start = millis();
		while(len){
			size_t toRead = len;
			if(toRead > 512){
				toRead = 512;
			}
			file.read(buf, toRead);
			len -= toRead;
		}
		end = millis() - start;
		Serial.printf("%u bytes read for %lu ms\n", flen, end);
		file.close();
	} else {
		Serial.println("Failed to open file for reading");
	}


	file = fs.open(path, FILE_WRITE);
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}

	size_t i;
	start = millis();
	for(i=0; i<2048; i++){
		file.write(buf, 512);
	}
	end = millis() - start;
	Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
	file.close();
}

EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(&hspi, HSPI_CS);

void setup(){
	Serial.begin(9600);

	hspi.begin();
	pinMode(HSPI_CS, OUTPUT); //HSPI SS

	if(myEEP.init()){
		Serial.println("EEPROM connected");
	}
	else{
		Serial.println("EEPROM does not respond");
		while(1);
	}
	/* You can change the SPI clock speed. The default of is 8 MHz */
	//myEEP.setSPIClockSpeed(4000000); // use AFTER init()!

	/* Select the page size of your EEPROM.
	 * Choose EEPROM_PAGE_SIZE_xxx,
	 * with xxx = 16, 32, 64, 128 or 256
	 */
	myEEP.setPageSize(EEPROM_PAGE_SIZE_32);

	byte byteToWrite = 42;
	myEEP.write(10, byteToWrite);  // write a byte to EEPROM address 10
	byte byteToRead = myEEP.read(10);
	Serial.print("Byte read: ");
	Serial.println(byteToRead);

	int intToWrite = -4242;
	int intToRead = 0;
	myEEP.put(10, intToWrite); // write an integer to EEPROM address 10
	myEEP.get(10, intToRead);
	Serial.print("Integer read: ");
	Serial.println(intToRead);

	float floatToWrite = 42.42;
	float floatToRead = 0.0;
	myEEP.put(10, floatToWrite);
	myEEP.get(10, floatToRead);
	Serial.print("Float read: ");
	Serial.println(floatToRead);

	char charArrayToWrite[] = "This is no poetry, I am just a simple char array";
	myEEP.put(110, charArrayToWrite);  // write stringToWrite to address 110
	char charArrayToRead[60] = "";  // reserve sufficient space
	myEEP.get(110, charArrayToRead);
	Serial.print("Char array read: ");
	Serial.println(charArrayToRead);

	String stringToWrite = "Hello, I am a test string";
	unsigned int nextAddr = myEEP.putString(200, stringToWrite);   // String objects need a different put function

	String stringToRead = "";
	myEEP.getString(200, stringToRead);
	Serial.print("String read: ");
	Serial.println(stringToRead);
	Serial.print("Next free address: ");
	Serial.println(nextAddr);

	int intArrayToWrite[20];
	int intArrayToRead[20];
	for(unsigned int i=0; i<(sizeof(intArrayToWrite)/sizeof(int)); i++){
		intArrayToWrite[i] = 10*i;
	}
	myEEP.put(250, intArrayToWrite);
	myEEP.get(250, intArrayToRead);
	for(unsigned int i=0; i<(sizeof(intArrayToRead)/sizeof(int)); i++){
		Serial.print("intArrayToRead[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println(intArrayToRead[i]);
	}

	display.begin();
	display.fillScreen(WHITE);
	display.setCursor(20, 20);
	display.setTextSize(2);
	display.setTextColor(BLUE);
	display.print("Hello world");

	if(!SD.begin(17)){ //Change to this function to manually change CS pin
		//if(!SD.begin()){
		Serial.println("Card Mount Failed");
		return;
	}
	uint8_t cardType = SD.cardType();

	if(cardType == CARD_NONE){
		Serial.println("No SD card attached");
		return;
	}

	Serial.print("SD Card Type: ");
	if(cardType == CARD_MMC){
		Serial.println("MMC");
	} else if(cardType == CARD_SD){
		Serial.println("SDSC");
	} else if(cardType == CARD_SDHC){
		Serial.println("SDHC");
	} else {
		Serial.println("UNKNOWN");
	}

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);

	listDir(SD, "/", 0);
	createDir(SD, "/mydir");
	listDir(SD, "/", 0);
	removeDir(SD, "/mydir");
	listDir(SD, "/", 2);
	writeFile(SD, "/hello.txt", "Hello ");
	appendFile(SD, "/hello.txt", "World!\n");
	readFile(SD, "/hello.txt");
	deleteFile(SD, "/foo.txt");
	renameFile(SD, "/hello.txt", "/foo.txt");
	readFile(SD, "/foo.txt");
	testFileIO(SD, "/test.txt");
	Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
	Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop() {
	display.fillScreen(RED);
	delay(1000);
	int intToWrite = -4242;
	int intToRead = 0;
	myEEP.put(10, intToWrite); // write an integer to EEPROM address 10
	myEEP.get(10, intToRead);
	Serial.print("Integer read: ");
	Serial.println(intToRead);
	display.fillScreen(GREEN);
	delay(1000);
	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);

	listDir(SD, "/", 0);
	createDir(SD, "/mydir");
	listDir(SD, "/", 0);
	removeDir(SD, "/mydir");
	listDir(SD, "/", 2);
	writeFile(SD, "/hello.txt", "Hello ");
	appendFile(SD, "/hello.txt", "World!\n");
	readFile(SD, "/hello.txt");
	deleteFile(SD, "/foo.txt");
	renameFile(SD, "/hello.txt", "/foo.txt");
	readFile(SD, "/foo.txt");
	testFileIO(SD, "/test.txt");
	Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
	Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}
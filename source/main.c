//-----------------------------------------------------------------------------------------
// Title:	GameBoy Tool CLI
// Program: GameBoy Tool
// Authors: Stephen Monn
//-----------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "spi.h"
#include "egpio.h"
#include "gbx.h"

#define SPI_CLK_SPEED 10000000

//defaults
static const char* defaultFunc = "none";
static const char* romExGB = "gb";
static const char* romExGBC = "gbc";
static const char* romExGBA = "gba";
static const char* saveExGB = "srm";
static const char* saveExGBC = "srm";
static const char* saveExGBA = "sav";

//functions
static void readROM(const char* outfilename);
static void readSave(const char* outfilename);
static void writeSave(const char* infilename);

//string utils
static void str_replace(char* str, char find, char to);
static void str_remove(char* str, char find);
static void str_fileSanitize(char* str);

//program entry
int main(int argc, char *argv[])
{
	int i;
	
	//init core modules
	if(spi_init(SPI_CLK_SPEED) || egpio_init() || gbx_init()) {
		printf("Init failed. Are you running as root??\n");
		gbx_close();
		egpio_close();
		spi_close();
		return 1;
	}
	
	//get params
	const char* func = defaultFunc;
	const char* outfilename = 0;
	const char* infilename = 0;
	if(argc > 1) func = argv[1];
	for(i=2; i<argc-1; i++) {
		if(strcasecmp(argv[i], "-o") == 0) outfilename = argv[(++i)];
		else if(strcasecmp(argv[i], "-i") == 0) infilename = argv[(++i)];
	}
	
	//load cartridge header
	gbx_loadHeader();
	
	//execute func
	if(strcasecmp(func, "info") == 0) {
		gbx_printInfo();
	} else if(strcasecmp(func, "readROM") == 0) {
		readROM(outfilename);
	} else if(strcasecmp(func, "readSave") == 0) {
		readSave(outfilename);
	} else if(strcasecmp(func, "writeSave") == 0) {
		writeSave(infilename);
	} else {
		printf("Available commands:\n");
		printf("   info\n");
		printf("   readROM -o <output_filename>\n");
		printf("   readSave -o <output_filename>\n");
		printf("   writeSave -i <input_filename>\n");
	}
	printf("\n");
	
	//close down
	gbx_close();
	egpio_close();
	spi_close();
	return 0;
}

//read rom data from cartridge
static void readROM(const char* outfilename)
{
	//check for cartridge
	if(gbx_getROMSize() == 0) {
		printf("No Cartridge...\n");
		return;
	}
	char type = gbx_getCartridgeType();
	if(type == GBX_CARTRIDGE_TYPE_GB) printf("Found GB cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBC) printf("Found GBC cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBA) printf("Found GBA cartridge [%s]\n", gbx_getGameTitle());
	
	//determine output filename
	char customfilename[512];
	if(outfilename == 0) {
		if(type == GBX_CARTRIDGE_TYPE_GB) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), romExGB);
		else if(type == GBX_CARTRIDGE_TYPE_GBC) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), romExGBC);
		else if(type == GBX_CARTRIDGE_TYPE_GBA) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), romExGBA);
		str_fileSanitize(customfilename);
		outfilename = customfilename;
	}
	
	//open file for writting
	FILE* file = fopen(outfilename, "w");
	if(file == NULL) {
		printf("Failed to open file for writing\n");
		return;
	}
	
	//get rom data
	printf("Reading ROM data... ");
	fflush(stdout);
	clock_t begin = clock();
	char* data = malloc(gbx_getROMSize());
	int readSize = gbx_readROM(data);
	printf("%fs\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	if(readSize < 0 || readSize != gbx_getROMSize()) printf("Error reading ROM\n");
	
	//write data to file
	else {
		int i;
		printf("Writting to file... ");
		fflush(stdout);
		clock_t begin = clock();
		for(i=0; i<readSize; i++) fputc(data[i], file);
		printf("%fs\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	}
	
	//close file and free data
	free(data);
	fclose(file);
}

//read save data from cartridge
static void readSave(const char* outfilename)
{
	//check for cartridge
	if(gbx_getROMSize() == 0) {
		printf("No Cartridge...\n");
		return;
	}
	char type = gbx_getCartridgeType();
	if(type == GBX_CARTRIDGE_TYPE_GB) printf("Found GB cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBC) printf("Found GBC cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBA) printf("Found GBA cartridge [%s]\n", gbx_getGameTitle());
	
	//check for save data
	if(gbx_getSaveSize() == 0) {
		printf("No Save on cartridge\n");
		return;
	}
	
	//determine output filename
	char customfilename[512];
	if(outfilename == 0) {
		if(type == GBX_CARTRIDGE_TYPE_GB) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), saveExGB);
		else if(type == GBX_CARTRIDGE_TYPE_GBC) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), saveExGBC);
		else if(type == GBX_CARTRIDGE_TYPE_GBA) sprintf(customfilename, "%s.%s", gbx_getGameTitle(), saveExGBA);
		str_fileSanitize(customfilename);
		outfilename = customfilename;
	}
	
	//open file for writting
	FILE* file = fopen(outfilename, "w");
	if(file == NULL) {
		printf("Failed to open file for writing\n");
		return;
	}
	
	//get save data
	printf("Reading Save data... ");
	fflush(stdout);
	clock_t begin = clock();
	char* data = malloc(gbx_getSaveSize());
	int readSize = gbx_readSave(data);
	printf("%fs\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	if(readSize < 0 || readSize != gbx_getSaveSize()) printf("Error reading Save\n");
	
	//write data to file
	else {
		int i;
		printf("Writting to file... ");
		fflush(stdout);
		clock_t begin = clock();
		for(i=0; i<readSize; i++) fputc(data[i], file);
		printf("%fs\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	}
	
	//close file and free data
	free(data);
	fclose(file);
}

//write save data to cartridge
static void writeSave(const char* infilename)
{
	//check for cartridge
	if(gbx_getROMSize() == 0) {
		printf("No Cartridge...\n");
		return;
	}
	char type = gbx_getCartridgeType();
	if(type == GBX_CARTRIDGE_TYPE_GB) printf("Found GB cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBC) printf("Found GBC cartridge [%s]\n", gbx_getGameTitle());
	else if(type == GBX_CARTRIDGE_TYPE_GBA) printf("Found GBA cartridge [%s]\n", gbx_getGameTitle());
	
	//check for save data
	if(gbx_getSaveSize() == 0) {
		printf("No Save on cartridge\n");
		return;
	}
	
	//open file for reading
	FILE* file = fopen(infilename, "rb");
	if(file == NULL) {
		printf("Failed to open file for reading\n");
		return;
	}
	
	//check file data size
	fseek(file, 0, SEEK_END);
	long savefilelen = ftell(file);
	rewind(file);
	if(savefilelen != gbx_getSaveSize()) {
		printf("File size and Save size do not match\n");
		fclose(file);
		return;
	}
	
	//get data from file
	printf("Reading from file... ");
	fflush(stdout);
	clock_t begin = clock();
	char* data = malloc(gbx_getSaveSize());
	fread(data, savefilelen, 1, file);
	fclose(file);
	printf("%fs\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	
	//write save data
	printf("Writting Save data... ");
	fflush(stdout);
	clock_t beginw = clock();
	int writeSize = gbx_writeSave(data);
	printf("%fs\n", (double)(clock() - beginw) / CLOCKS_PER_SEC);
	if(writeSize < 0 || writeSize != gbx_getSaveSize()) printf("Error writting Save\n");
	
	//free data
	free(data);
}

//string utils
static void str_replace(char* str, char find, char to) {
	for(int i=0; str[i]!=0; i++) if(str[i]==find) str[i]=to;
}
static void str_remove(char* str, char find) {
	for(int i=0; str[i]!=0; i++) if(str[i]==find) for(int j=0; str[i+j]!=0; j++) str[i+j]=str[i+j+1];
}
static void str_fileSanitize(char* str) {
	str_remove(str, '<');
	str_remove(str, '>');
	str_remove(str, '"');
	str_remove(str, ':');
	str_remove(str, '/');
	str_remove(str, '\\');
	str_remove(str, '|');
	str_remove(str, '?');
	str_remove(str, '*');
	str_replace(str, ' ', '_');
}

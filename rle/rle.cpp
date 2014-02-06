// rle.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "stdafx.h"
#include "rletest.h"
#include "rle-algo.hpp"
#include "rle-files.h"

void Part1Tests()
{
	std::cout << "Testing compression..." << std::endl;
	RLE<char> r;
	r.Compress(CHAR_1, strlen(CHAR_1));
	std::cout << r << std::endl;
	r.Compress(CHAR_2, strlen(CHAR_2));
	std::cout << r << std::endl;
	r.Compress(CHAR_3, strlen(CHAR_3));
	std::cout << r << std::endl;
	r.Compress(CHAR_4, strlen(CHAR_4));
	std::cout << r << std::endl << std::endl;
	
	std::cout << "Testing decompression..." << std::endl;
	r.Decompress(RLE_CHAR_1, strlen(RLE_CHAR_1), 40);
	std::cout << r << std::endl;
	r.Decompress(RLE_CHAR_2, strlen(RLE_CHAR_2), 12);
	std::cout << r << std::endl;
	r.Decompress(RLE_CHAR_3, strlen(RLE_CHAR_3), 14);
	std::cout << r << std::endl << std::endl;
}


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "Beginning RLE lab..." << std::endl;
	std::string input = "";
	std::string filename = "";
	
	while(input.compare("") == 0){
		std::cout << "Select an option:" << std::endl;
		std::cout << "	1. Part 1 (Tests)" << std::endl;
		std::cout << "	2. Part 2 (Single Files)" << std::endl;
		std::cout << "	3. Part 3 (Directories)" << std::endl;
		std::cout << "	4. Exit" << std::endl;
	
		std::getline (std::cin, input);
		if(input.find("1") != std::string::npos){
			std::cout << "Option 1 selected." << std::endl;
			Part1Tests();
		}
		else if(input.find("2") != std::string::npos){
			std::cout << "Option 2 selected." << std::endl;
			input = "";
			while (input.compare("") == 0){
				std::cout << "Select an option:" << std::endl;
				std::cout << "	1. Create an archive" << std::endl;
				std::cout << "	2. Extract an archive" << std::endl;
				std::cout << "	3. Return to main menu" << std::endl;

				std::getline(std::cin, input);
				if (input.find("1") != std::string::npos){
					std::cout << "Option 1 selected. Enter the name of the file to be archived: ";
					filename = "";
					std::getline(std::cin, filename);
					RLE_v1 r;
					r.CreateArchive(filename);
				}
				else if (input.find("2") != std::string::npos){
					std::cout << "Option 2 selected. Enter the name of the file to be extracted: ";
					filename = "";
					std::getline(std::cin, filename);
					RLE_v1 r;
					r.ExtractArchive(filename);
				}
				else if (input.find("3") != std::string::npos){
					std::cout << "Option 3 selected." << std::endl;
					break;
				}
				else {
					std::cout << "Please enter a value between 1 and 3 to select an option. Restarting menu..." << std::endl;
				}
				input = "";
			}
		}
		else if(input.find("3") != std::string::npos){
			std::cout << "Option 3 selected." << std::endl;
			input = "";
			while (input.compare("") == 0){
				std::cout << "Select an option:" << std::endl;
				std::cout << "	1. Create an archive" << std::endl;
				std::cout << "	2. Extract an archive" << std::endl;
				std::cout << "	3. Return to main menu" << std::endl;

				std::getline(std::cin, input);
				if (input.find("1") != std::string::npos){
					std::cout << "Option 1 selected. Enter the name of the directory to be archived: ";
					filename = "";
					std::getline(std::cin, filename);
					RLE_v2 r;
					r.CreateArchive(filename);
				}
				else if (input.find("2") != std::string::npos){
					std::cout << "Option 2 selected. Enter the name of the directory to be extracted: ";
					filename = "";
					std::getline(std::cin, filename);
					RLE_v2 r;
					r.ExtractArchive(filename);
				}
				else if (input.find("3") != std::string::npos){
					std::cout << "Option 3 selected." << std::endl;
					break;
				}
				else {
					std::cout << "Please enter a value between 1 and 3 to select an option. Restarting menu..." << std::endl;
				}
				input = "";
			}
		}
		else if(input.find("4") != std::string::npos){
			std::cout << "Option 4 selected." << std::endl;
			std::cout << "Quitting..." << std::endl;
			return 0;
		}
		else {
			std::cout << "Please enter a value between 1 and 4 to select an option. Restarting menu..." << std::endl;
		}
		input = "";
	}
	return 0;
}


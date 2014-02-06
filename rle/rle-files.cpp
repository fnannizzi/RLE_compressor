#include "rle-files.h"
#include "rle-algo.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <windows.h>

void RLE_v1::CreateArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* data;
	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);
	
	// Attempt to open the file and read the data
	if (file.is_open())
	{
		size = file.tellg();
		data = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(data, size);
		file.close();
	}
	else 
	{
		std::cout << "Bad filename! Returning to menu..." << std::endl;
		return;
	}

	// Initialize the special RLE1 signature
	SetSignature();

	// Initialize the header information
	m_Header.fileSize = static_cast<int>(size);
	m_Header.fileNameLength = sizeof(source);
	m_Header.fileName = source;
	m_Data.Compress(data, static_cast<int>(size));

	// Create and write the archive file
	WriteDataToArchive(source);

	delete[] data;
}

void RLE_v1::ExtractArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* data;
	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);
	
	// Attempt to open the file and read the data
	if (file.is_open())
	{
		size = file.tellg();
		data = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(data, size);
		file.close();
	}
	else 
	{
		std::cout << "Invalid archive! Returning to menu..." << std::endl;
		return;
	}

	// Begin reading header information
	for (int bit = 0; bit < 4; bit++)
	{
		m_Header.sig[bit] = data[bit];
	}

	// Validate the file signature
	if ((m_Header.sig[0] != 'R') || (m_Header.sig[1] != 'L') || (m_Header.sig[2] != 'E') || (m_Header.sig[3] != '\x01'))
	{
		std::cout << "File not compressed using RL1! Ending decompression..." << std::endl;
		delete[] data;
		return;
	}

	// Continue reading header information
	m_Header.fileSize = *(reinterpret_cast<int*>(&data[4]));
	m_Header.fileNameLength = *(reinterpret_cast<unsigned char*>(&data[8]));

	char *fileNameData = new char[int(m_Header.fileNameLength) + 1];
	for (int bit = 0; bit < int(m_Header.fileNameLength); bit++)
	{
		fileNameData[bit] = data[9 + bit];
	}
	fileNameData[m_Header.fileNameLength] = '\0'; // add null terminator to filename
	std::string str(fileNameData);
	m_Header.fileName = fileNameData;

	// Do the decompression on the remaining data
	m_Data.Decompress(&data[9 + m_Header.fileNameLength], (static_cast<int>(size) - (9 + m_Header.fileNameLength)), m_Header.fileSize);

	// Write the unarchived file
	std::ofstream unarc(m_Header.fileName, std::ios::out | std::ios::binary | std::ios::trunc); if (unarc.is_open())
	{
		unarc.write(m_Data.m_Data, m_Data.m_Size);
	}

	std::cout << "File extracted to " << m_Header.fileName << "." << std::endl;
	delete[] data;
}

void RLE_v1::SetSignature()
{
	m_Header.sig[0] = 'R';
	m_Header.sig[1] = 'L';
	m_Header.sig[2] = 'E';
	m_Header.sig[3] = '\x01';
}

void RLE_v1::WriteDataToArchive(std::string filename)
{
	filename.append(".rl1");

	std::ofstream arc(filename, std::ios::out | std::ios::binary | std::ios::trunc); if (arc.is_open())
	{
		arc.write(m_Header.sig, 4);
		arc.write(reinterpret_cast<char*>(&(m_Header.fileSize)), 4);
		arc.write(reinterpret_cast<char*>(&(m_Header.fileNameLength)), 1);
		arc.write(m_Header.fileName.c_str(), m_Header.fileNameLength);
		arc.write(m_Data.m_Data, m_Data.m_Size);
	}

	std::cout << "File archived to " << filename << "." << std::endl;
}

void RLE_v2::CreateArchive(const std::string& source)
{
	// Find the first file in the directory.
	WIN32_FIND_DATA ffd;
	std::string path = source + "\\*.*";
	HANDLE hFind = FindFirstFile(path.c_str(), &ffd); if (INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << "Bad directory name! Returning to menu..." << std::endl;
		return;
	}
	// Add all files in this directory to a list of strings
	std::list<std::string> files;
	do
	{
		std::string temp = ffd.cFileName;
		if (temp != "." && temp != ".." &&
			!(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			files.push_back(ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	// Initialize the special RLE2 signature
	SetSignature();

	// Initialize header information
	m_Header.dirNameLength = sizeof(source);
	m_Header.dirName = source;

	// Begin opening and reading the files to be archived
	for (std::list<std::string>::const_iterator iterator = files.begin(); iterator != files.end(); ++iterator)
	{
		std::string filepath = source;
		filepath.append("/");
		filepath.append(*iterator);
		
		std::ifstream::pos_type size;
		char* data;
		std::ifstream file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			data = new char[static_cast<unsigned int>(size)];
			file.seekg(0, std::ios::beg);
			file.read(data, size);
			file.close();
		}
		else 
		{
			std::cout << "Can't archive file " << filepath << ". Archiving other files..." << std::endl;
			continue;
		}

		file_info *f_Info = new file_info;

		f_Info->fileSize = static_cast<int>(size);
		f_Info->fileName = (*iterator);
		f_Info->fileNameLength = sizeof(*iterator);
		f_Info->data.Compress(data, static_cast<int>(size));
		f_Info->compressedSize = f_Info->data.m_Size;

		m_Files.push_back(f_Info);
	}
	
	// Write the data to an archive
	WriteDataToArchive(source);
}

void RLE_v2::ExtractArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* data;
	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);

	// Attempt to open file and read data
	if (file.is_open())
	{
		size = file.tellg();
		data = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(data, size);
		file.close();
	}
	else 
	{
		std::cout << "Invalid archive! Returning to menu..." << std::endl;
		return;
	}

	// Begin reading in header information
	for (int bit = 0; bit < 4; bit++)
	{
		m_Header.sig[bit] = data[bit];
	}

	// Validate the file signature
	if ((m_Header.sig[0] != 'R') || (m_Header.sig[1] != 'L') || (m_Header.sig[2] != 'E') || (m_Header.sig[3] != '\x02'))
	{
		std::cout << "Directory not compressed using RL2! Ending decompression..." << std::endl;
		return;
	}

	// Continue reading header information
	m_Header.dirNameLength = *(reinterpret_cast<unsigned char*>(&data[4]));
	char *dirNameData = new char[int(m_Header.dirNameLength) + 1];
	for (int bit = 0; bit < int(m_Header.dirNameLength); bit++){
		dirNameData[bit] = data[5 + bit];
	}
	dirNameData[m_Header.dirNameLength] = '\0'; // add null terminator to the directory name
	std::string str(dirNameData);
	m_Header.dirName = dirNameData;

	// If the directory doesn't already exist, create a new one
	CreateDirectory(m_Header.dirName.c_str(), NULL);

	// Create a new pointer to track where we are while reading the data
	char* data_Ptr = &data[5 + m_Header.dirNameLength];

	// While the data pointer is within the range of bytes we read, read files
	while (data_Ptr < (data + static_cast<int>(size)))
	{
		file_info *f_Info = new file_info;

		// Read file header data
		f_Info->fileSize = *(reinterpret_cast<int*>(&data_Ptr[0]));
		f_Info->compressedSize = *(reinterpret_cast<int*>(&data_Ptr[4]));
		f_Info->fileNameLength = *(reinterpret_cast<unsigned char*>(&data_Ptr[8]));
		char *fileNameData = new char[int(f_Info->fileNameLength) + 1];

		for (int bit = 0; bit < int(f_Info->fileNameLength); bit++)
		{
			fileNameData[bit] = data_Ptr[9 + bit];
		}
		fileNameData[f_Info->fileNameLength] = '\0';

		std::string str(fileNameData);
		f_Info->fileName = fileNameData;

		// Do the decompression
		f_Info->data.Decompress(&data_Ptr[9 + f_Info->fileNameLength], f_Info->compressedSize, f_Info->fileSize);
		data_Ptr = &data_Ptr[9 + f_Info->fileNameLength + f_Info->compressedSize];

		std::string filepath = m_Header.dirName;
		filepath.append("/");
		filepath.append(f_Info->fileName);

		// Write the unarchived file
		std::ofstream unarc(filepath, std::ios::out | std::ios::binary | std::ios::trunc); if (unarc.is_open())
		{
			unarc.write(f_Info->data.m_Data, f_Info->data.m_Size);
		}

		std::cout << "File extracted to " << f_Info->fileName << "." << std::endl;
	}

	std::cout << "Directory extracted to " << m_Header.dirName << "." << std::endl;
	delete[] data;
}

void RLE_v2::SetSignature()
{
	m_Header.sig[0] = 'R';
	m_Header.sig[1] = 'L';
	m_Header.sig[2] = 'E';
	m_Header.sig[3] = '\x02';
}

void RLE_v2::WriteDataToArchive(std::string filename)
{
	filename.append(".rl2");

	std::ofstream arc(filename, std::ios::out | std::ios::binary | std::ios::trunc); if (arc.is_open())
	{
		arc.write(m_Header.sig, 4);
		arc.write(reinterpret_cast<char*>(&(m_Header.dirNameLength)), 1);
		arc.write(m_Header.dirName.c_str(), m_Header.dirNameLength);

		for (std::list<file_info*>::const_iterator iterator = m_Files.begin(); iterator != m_Files.end(); ++iterator){
			arc.write(reinterpret_cast<char*>(&((*iterator)->fileSize)), 4);
			arc.write(reinterpret_cast<char*>(&((*iterator)->compressedSize)), 4);
			arc.write(reinterpret_cast<char*>(&((*iterator)->fileNameLength)), 1);
			arc.write((*iterator)->fileName.c_str(), (*iterator)->fileNameLength);
			arc.write((*iterator)->data.m_Data, (*iterator)->data.m_Size);
		}
	}

	std::cout << "Directory archived to " << filename << "." << std::endl;
}

RLE_v2::~RLE_v2()
{
	m_Files.clear();
}

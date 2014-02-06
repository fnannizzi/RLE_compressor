#pragma once
#include "rle-algo.hpp"
#include <string>
#include <list>

// RLE v1 archive supports compressing a single file
class RLE_v1
{
	struct header
	{
		char sig[4];
		int fileSize;
		unsigned char fileNameLength;
		std::string fileName;
	} m_Header;

	RLE<char> m_Data;

public:
	void CreateArchive(const std::string& source);

	void ExtractArchive(const std::string& source);

	void SetSignature();

	void WriteDataToArchive(std::string filename);
};

// RLE v2 archive supports compressing all the files a
// directory.
class RLE_v2
{
	struct header
	{
		char sig[4];
		unsigned char dirNameLength;
		std::string dirName;
	} m_Header;

	struct file_info
	{
		int fileSize;
		int compressedSize;
		unsigned char fileNameLength;
		std::string fileName;
		RLE<char> data;
	};

	std::list<file_info*> m_Files;

public:
	~RLE_v2();
	void CreateArchive(const std::string& source);

	void ExtractArchive(const std::string& source);

	void SetSignature();

	void WriteDataToArchive(std::string filename);

};

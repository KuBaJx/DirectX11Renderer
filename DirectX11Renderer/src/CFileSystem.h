#pragma once

#include <iostream>
#include <string>
#include <fstream>

class CFileSystem
{
private:
	std::ofstream m_file;
public:
	CFileSystem(const char*);
	CFileSystem(const CFileSystem&);
	~CFileSystem();

	void WriteToFile(std::string&);
};


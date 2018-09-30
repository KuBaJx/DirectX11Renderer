#pragma once

#include <iostream>
#include <fstream>

class CFileSystem
{
private:
	std::ofstream m_file;
public:
	CFileSystem(const char*);
	CFileSystem(const CFileSystem&);
	~CFileSystem();

	void WriteToFile(const char*);
};


#include "CFileSystem.h"

CFileSystem::CFileSystem(const char* filename)
{
	m_file.open(filename);
	if (!m_file.is_open())
	{
		std::cerr << "Failed to open file!";
	}
}

CFileSystem::CFileSystem(const CFileSystem& other)
{

}

CFileSystem::~CFileSystem()
{
	m_file.close();
}

void CFileSystem::WriteToFile(const char* msg)
{
	m_file << "[GPU NAME]: " << msg << std::endl;
}

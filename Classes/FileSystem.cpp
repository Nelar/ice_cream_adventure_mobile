#include "FileSystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

FileSystem::FileSystem()
{
	getWorkingDirectory();
}

bool FileSystem::isFileExists(const char* fileName)
{
	ReadFileDisk* file = new ReadFileDisk(fileName);
	bool isFile = file->isOpen();	
	return isFile;
}

ReadFileDisk* FileSystem::openReadFile(const char* fileName)
{
	ReadFileDisk* file = new ReadFileDisk(fileName);
	if (file->isOpen())
		return file;	
	return NULL;
}

WriteFileDisk* FileSystem::openWriteFile(const char* fileName, bool append)
{
	WriteFileDisk* file = new WriteFileDisk(fileName,append);
	if (file->isOpen())
		return file;	
	return NULL;
}

const char*  FileSystem::getWorkingDirectory()
{
	char workDir[FILE_SYSTEM_MAX_PATH];
	getcwd(workDir, FILE_SYSTEM_MAX_PATH);
	return workDir;
}

bool  FileSystem::changeWorkingDirectoryTo(const char* newDirectory)
{
	bool success=false;
	success=(chdir(newDirectory) == 0);
	return success;
}

string	FileSystem::getFileDir(const string& filename)
{	
	int lastSlash = filename.find_last_of('/');
	const int lastBackSlash = filename.find_last_of('\\');
	lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;

	if ((unsigned int)lastSlash < filename.size())
		return filename.substr(0, lastSlash);
	else
		return ".";
}

ReadFileDisk::ReadFileDisk(const char* fileName) : handle(0), size(0)
{
	name = fileName;
	openFile();
}

ReadFileDisk::~ReadFileDisk()
{
	if (handle)
		fclose(handle);
}

bool ReadFileDisk::isOpen() const
{
	return handle!=0;
}

unsigned int ReadFileDisk::getPos() const
{
	return ftell(handle);
}

unsigned int ReadFileDisk::getSize() const
{
	return size;
}

unsigned int ReadFileDisk::read(void* buffer, unsigned int sizeToRead)
{
	if (!isOpen())
		return 0;

	return (unsigned int)fread(buffer, 1, sizeToRead, handle);
}

char ReadFileDisk::readChar()
{
	char value;
	read(&value, 1);
	return value;
}

unsigned char ReadFileDisk::readU8()
{
	unsigned char value;
	read(&value, 1);
	return value;
}

signed char ReadFileDisk::readS8()
{
	signed char value;
	read(&value, 1);
	return value;
}

unsigned short ReadFileDisk::readU16()
{
	unsigned short value;
	read(&value, 2);
	return value;
}

short ReadFileDisk::readS16()
{
	short value;
	read(&value, 2);
	return value;
}

unsigned int ReadFileDisk::readU32()
{
	unsigned int value;
	read(&value, 4);
	return value;
}

int ReadFileDisk::readS32()
{
	int value;
	read(&value, 4);
	return value;
}

float ReadFileDisk::readFloat32()
{
	float value;
	read(&value, 4);
	return value;
}

bool ReadFileDisk::readBool()
{
	bool value;
	read(&value, 1);
	return value;
}

bool ReadFileDisk::seek(unsigned int pos, bool relative)
{
	if (!isOpen())
		return false;

	return fseek(handle, pos, relative ? SEEK_CUR:SEEK_SET) == 0;
}

const char* ReadFileDisk::getFileName() const
{
	return name.c_str();
}

void ReadFileDisk::openFile()
{
	if (name.size() == 0)
	{
		handle = 0;
		return;
	}

	handle = fopen(name.c_str(),"rb");

	if (handle)
	{
		fseek(handle, 0, SEEK_END);
		size = ftell(handle);
		fseek(handle, 0, SEEK_SET);
	}	
}

void ReadFileDisk::close()
{
	fclose(handle);
}

WriteFileDisk::WriteFileDisk(const char* fileName, bool append) : handle(0)
{
	name = fileName;
	openFile(append);
}

WriteFileDisk::~WriteFileDisk()
{
	if (handle)
		fclose(handle);
}

bool WriteFileDisk::isOpen() const
{
	return handle!=0;
}

unsigned int WriteFileDisk::getPos() const
{
	return (unsigned int)ftell(handle);
}

unsigned int WriteFileDisk::write(void* buffer, unsigned int sizeToWrite)
{
	if (!isOpen())
		return 0;

	return (unsigned int)fwrite(buffer, 1, sizeToWrite, handle);
}

void WriteFileDisk::write(char value)
{
	write(&value, 1);
}

void WriteFileDisk::write(unsigned char value)
{
	write(&value, 1);
}

void WriteFileDisk::write(signed char value)
{
	write(&value, 1);
}

void WriteFileDisk::write(unsigned short value)
{
	write(&value, 2);
}

void WriteFileDisk::write(short value)
{
	write(&value, 2);
}

void WriteFileDisk::write(unsigned int value)
{
	write(&value, 4);
}


void WriteFileDisk::write(int value)
{
	write(&value, 4);
}

void WriteFileDisk::write(float value)
{
	write(&value, 4);
}

void WriteFileDisk::write(bool value)
{
	write(&value, 1);
}

bool WriteFileDisk::seek(unsigned int pos, bool relative)
{
	if (!isOpen())
		return false;

	return fseek(handle, pos, relative ? SEEK_CUR : SEEK_SET) == 0;
}

const char* WriteFileDisk::getFileName() const
{
	return name.c_str();
}

void WriteFileDisk::openFile(bool append)
{
	if (name.size() == 0)
	{
		handle = 0;
		return; 
	}

	handle = fopen(name.c_str(), append ? "ab" : "wb");
}

void WriteFileDisk::close()
{
	fclose(handle);
}
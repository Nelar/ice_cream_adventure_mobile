#ifndef _CFILESYS_H_
#define _CFILESYS_H_

#include <stdio.h>
#include <vector>
#include <string>
#include "cSingleton.h"

using namespace std;

const int FILE_SYSTEM_MAX_PATH = 1024;

class ReadFile
{
public:
	virtual ~ReadFile() {}

	virtual unsigned int	getPos() const = 0;
	virtual unsigned int	getSize() const = 0;

	virtual unsigned int	read(void* buffer, unsigned int sizeToRead) = 0;
	virtual bool	seek(unsigned int pos, bool relative = false) = 0;

	virtual const char* getFileName() const = 0;
};

class ReadFileDisk : public ReadFile
{
public:
	ReadFileDisk(const char* fileName);
	virtual ~ReadFileDisk();

	bool isOpen() const;
	void close();

	virtual unsigned int getPos() const;
	virtual unsigned int getSize() const;

	virtual unsigned int read(void* buffer, unsigned int sizeToRead);
	virtual bool	seek(unsigned int pos, bool relative = false);

	unsigned char readU8();
	signed char readS8();
	unsigned short readU16();
	short readS16();
	unsigned int readU32();
	int readS32();
	float readFloat32();
	bool readBool();
	char readChar();

	virtual const char* getFileName() const;

protected:
	FILE* handle;
	string name;
	int size;

private:
	void openFile();
};

class WriteFile
{
public:
	virtual ~WriteFile() {}

	virtual unsigned int		getPos() const = 0;

	virtual unsigned int		write(void* buffer, unsigned int sizeToWrite) = 0;
	virtual bool	seek(unsigned int pos, bool relative = false) = 0;

	virtual const char* getFileName() const = 0;
};

class WriteFileDisk : public WriteFile
{
public:
	WriteFileDisk(const char* fileName, bool append);
	virtual ~WriteFileDisk();

	bool isOpen() const;
	void close();

	virtual unsigned int		getPos() const;

	virtual unsigned int		write(void* buffer, unsigned int sizeToWrite);
	virtual bool	seek(unsigned int pos, bool relative = false);

	virtual const char* getFileName() const;

	void write(char value);
	void write(unsigned char value);
	void write(signed char value);
	void write(unsigned short value);
	void write(short value);
	void write(unsigned int value);
	void write(int value);
	void write(float value);
	void write(bool value);

protected:
	FILE* handle;
	string name;

private:
	void openFile(bool append);
};


class FileSystem
{
public:
	FileSystem();
	virtual ~FileSystem() {};

	ReadFileDisk* openReadFile(const char* fileName);
	WriteFileDisk* openWriteFile(const char* fileName, bool append=false);
	static bool isFileExists(const char* fileName);

	static  const char*	getWorkingDirectory();
	static  bool		changeWorkingDirectoryTo(const char* newDirectory);
	static  string		getFileDir(const string& filename);	
};

#endif
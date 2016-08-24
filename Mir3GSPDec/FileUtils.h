#pragma once


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <WinSock.h>
#else
#include <limits.h>
#include <dlfcn.h> //dlopen
//!MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH 1024 
#endif

#endif

#include <string>
#include <vector>
#include <time.h>
#include <cstdint>
#include <algorithm>
using namespace std;

namespace FileUtils
{
	//!获取标准目录路径
	string GetStandPath(const char* path);
	string GetStandFilePath(const char* filePath);
	bool FileDelete(const char* file);
	string GetFileExt(const char* filePath);
	string GetFileName(const char* filePath);


	string GetCurrentDir();

	bool IsDirectory(const char* path);
	bool DirExists(const char* path);
	bool DirCreate(const char* path);
	bool DirDelete(const char* path);
	bool DirCreateRecursion(const char* path);
	bool DirDeleteRecursion(const char* path);


	struct file_t
	{
		char filename[MAX_PATH];
		char fullpath[MAX_PATH];
		unsigned long long filesize;
		bool isDir;
	};
	//!
	bool SearchFiles(const char* path, vector<file_t> & files);
	template < typename OP >
	inline bool ForEachFiles(const char* path, const OP& op)
	{
		vector<file_t> files;
		if (SearchFiles(path, files))
		{
			std::for_each(files.begin(), files.end(), [=](file_t& file)
			{
				op(file.filename);
			}
			);
			return true;
		}
		return false;
	}
}
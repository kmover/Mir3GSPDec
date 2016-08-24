#include "StdAfx.h"
#include "FileUtils.h"

namespace FileUtils
{


	string GetStandPath(const char* path)
	{
		string p = path;
		for (auto iter = p.begin(); iter != p.end(); ++iter)
		{
			if (*iter == '\\')
			{
				*iter = '/';
			}
		}
		if (p.at(p.length() - 1) != '/')
		{
			p += "/";
		}
		return p;
	}
	string GetStandFilePath(const char* filePath)
	{
		string p = filePath;
		for (auto iter = p.begin(); iter != p.end(); ++iter)
		{
			if (*iter == '\\')
			{
				*iter = '/';
			}
		}
		return p;
	}

	string GetFileName(const char* filePath)
	{
		string path = GetStandFilePath(filePath);

		const char* ext = ::strrchr(path.c_str(), '/');
		if (ext != nullptr) {
			return ++ext;
		}
		return "";
	}
	string GetFileExt(const char* filePath)
	{
		const char* ext = ::strrchr(filePath, '.');
		if (ext != nullptr) {
			return ++ext;
		}
		return "";
	};

	string GetCurrentDir()
	{
#ifdef _WIN32 
		char tmp[MAX_PATH];
		::GetCurrentDirectoryA(MAX_PATH,tmp);
		return tmp;
#else

#error "not support this platform"
		//return ::getcwd();
#endif
	}

	bool FileDelete(const char* file)
	{
#ifdef _WIN32
		return ::DeleteFileA(file) != FALSE;
#else
		return remove(file) == 0;
#endif

	}
	bool IsDirectory(const char* path)
	{
#ifdef _WIN32
		WIN32_FILE_ATTRIBUTE_DATA fd = { 0 };
		if (0 == ::GetFileAttributesExA(path, ::GetFileExInfoStandard, &fd)) return false;
		if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) return true;
		return false;
#else
		struct _stat st;
		if (::_stat(path, &st) != 0) return false;
		if ((st.st_mode & S_IFDIR) == S_IFDIR) return true;

		return false;
#endif
	}


	bool DirExists(const char* path)
	{
#ifdef _WIN32
		WIN32_FILE_ATTRIBUTE_DATA fd = { 0 };
		if (0 == ::GetFileAttributesExA(path, ::GetFileExInfoStandard, &fd)) return false;
		if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) return true;
		return false;
#else
		struct stat st;
		if (stat(path, &st)) return false;
		if ((st.st_mode & S_IFDIR) == S_IFDIR) return true;
		return false;
#endif
	};
	bool DirCreate(const char* path)
	{
#ifdef WIN32
		return CreateDirectoryA(path, NULL) ? true : false;
#else
		return (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
	}
	bool DirDelete(const char* path)
	{
#ifdef WIN32
		return RemoveDirectoryA(path) ? true : false;
#else
		return (rmdir(path) == 0) ? true : false;
#endif
	}
	bool DirCreateRecursion(const char* path)
	{
		string p = GetStandPath(path);
		if (p.length() == 0) return true;
		string sub;
		string::size_type pos = p.find('/');
		while (pos != string::npos)
		{
			string cur = p.substr(0, pos - 0);
			if (cur.length() > 0 && !IsDirectory(cur.c_str()))
			{
				if (!DirCreate(cur.c_str()))
				{
					return false;
				}
			}
			pos = p.find('/', pos + 1);
		}

		return true;
	}
	bool DirDeleteRecursion(const char* path)
	{
		vector<file_t> files;
		SearchFiles(path, files);

		for (vector<file_t>::const_reverse_iterator iter = files.rbegin(); iter != files.rend(); ++iter)
		{
			if (iter->isDir)
			{
				DirDelete(iter->fullpath);
			}
			else
			{
				FileDelete(iter->fullpath);
			}
		}
		return DirDelete(path);
	}


	bool _SearchFiles(const char* p, vector<file_t>& files);

	bool SearchFiles(const char* p, vector<file_t> & files)
	{
		if (p == nullptr) return false;
		files.reserve(10);
		_SearchFiles(p, files);
		return true;
	}

	bool _SearchFiles(const char* p, vector<file_t>& files)
	{
		string path = GetStandPath(p);
#ifdef WIN32
		WIN32_FIND_DATAA fd;
		string findpath = path;
		findpath.append("*");
		HANDLE hFile = FindFirstFileA(findpath.c_str(), &fd);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		file_t file;
		do
		{
			if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)
				{
					memset(&file, 0, sizeof(file));
					file.isDir = true;
					::strncpy(file.filename,  fd.cFileName, sizeof(file.filename));
					::snprintf(file.fullpath, sizeof(file.fullpath), "%s%s", path.c_str(), fd.cFileName);
					files.push_back(file);
					_SearchFiles(file.fullpath, files);
				}
			}
			else
			{
				memset(&file, 0, sizeof(file));
				file.isDir = false;
				file.filesize = fd.nFileSizeHigh;
				file.filesize = file.filesize << 32;
				file.filesize += fd.nFileSizeLow;
				::strncpy(file.filename, fd.cFileName, sizeof(file.filename));
				::snprintf(file.fullpath, sizeof(file.fullpath), "%s%s", path.c_str(), fd.cFileName);
				files.push_back(file);
			}
		} while (FindNextFileA(hFile, &fd));
		FindClose(hFile);

#else
		DIR *dp;
		struct dirent *entry;
		struct stat statbuf;
		if ((dp = opendir(path.c_str())) == NULL)
		{
			return false;
		}
		file_t file;
		while ((entry = readdir(dp)) != NULL)
		{
			lstat(entry->d_name, &statbuf);
			if (S_ISDIR(statbuf.st_mode))
			{
				if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
				{
					continue;
				}
				memset(&file, 0, sizeof(file));
				file.isDir = true;
				file.filesize = statbuf.st_size;
				str::strncpy(file.filename, sizeof(file.filename), entry->d_name);
				str::sprintf(file.fullpath, "%s%s", path.c_str(), entry->d_name);
				files.push_back(file);
				_SearchFiles(file.fullpath, files);
			}
			else
			{
				memset(&file, 0, sizeof(file));
				file.isDir = false;
				file.filesize = statbuf.st_size;
				str::strncpy(file.filename, sizeof(file.filename), entry->d_name);
				file.fullpath[0] = '\0';
				files.push_back(file);
			}
		}
		closedir(dp);
#endif
		return true;
	}

}

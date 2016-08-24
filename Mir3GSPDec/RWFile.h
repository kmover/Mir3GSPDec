#pragma once
#include <string>
using std::string;

class RWFile
{
public:
	RWFile(void);
	~RWFile(void);
	static int GetFileSize(const string& _FileName);
	static bool LoadFile(const string& _FileName,string& _Buff);
	static bool SaveFile(const string& _FileName,const string& _Buff);
};

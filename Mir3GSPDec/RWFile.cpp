#include "StdAfx.h"
#include "RWFile.h"

RWFile::RWFile(void)
{
}

RWFile::~RWFile(void)
{
}

int RWFile::GetFileSize(const string& _FileName)
{
	int _FileSize = 0;
	FILE* _pBinFile = fopen(_FileName.c_str(), "rb");
	if(_pBinFile){
		fseek(_pBinFile, 0, SEEK_END);
		_FileSize = ftell(_pBinFile);
		fclose(_pBinFile);
	}
	return _FileSize;
}

bool RWFile::LoadFile(const string& _FileName,string& _Buff)
{
	bool _ret = false;
	FILE* _pBinFile = fopen(_FileName.c_str(), "rb");
	if(_pBinFile){
		fseek(_pBinFile, 0, SEEK_END);
		UINT _FileSize = ftell(_pBinFile);
		fseek(_pBinFile, 0, SEEK_SET);
		_Buff.resize(_FileSize);
		_ret = true;
		if(fread((void*)_Buff.c_str(), _FileSize, 1, _pBinFile) != 1) {
			_Buff = "";
			_ret = false;
		}
		fclose(_pBinFile);
	}
	return _ret;
}

bool RWFile::SaveFile(const string& _FileName,const string& _Buff)
{
	bool _ret = false;
	FILE* _pBinFile = fopen(_FileName.c_str(), "wb");
	if(_pBinFile){
		if(fwrite((void*)_Buff.c_str(), _Buff.size(), 1, _pBinFile) == 1) {
			_ret = true;
		}
		fclose(_pBinFile);
	}
	return _ret;
}
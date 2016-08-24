// decryptMono.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "RWFile.h"
#include "FileUtils.h"
#include <openssl/blowfish.h>

void DecryptItem(string path,string out);
int main(int argc, char **argv)
{
	if (argc == 2)
	{
		string path = argv[1];
		DecryptItem(path,path);
	}
	else {
		string cdir = FileUtils::GetCurrentDir();

		string outDir = cdir + "/Dec/";
		if (!::FileUtils::DirExists(outDir.c_str()))
		{
			FileUtils::DirCreate(outDir.c_str());
		}
		FileUtils::ForEachFiles(cdir.c_str(), [=](const char* file) {
			if (FileUtils::GetFileExt(file) == "dat")
			{
				::printf("Desc:%s\n", file);
				DecryptItem(file, outDir + file);
			}

		});
	}
	::system("pause");
	return 0;
}

void DecMir3ResDat(BYTE* data, DWORD len)
{
	BYTE* dst = data;
	string inData(len,'\0');
	BYTE* src = (BYTE*)inData.c_str();
	memcpy(src,dst,len);

	BF_KEY key = {0};
	char* userKey = "res070821mir";
	BF_set_key(&key, strlen(userKey), (const unsigned char *)userKey);

	int blockNum = len / BF_BLOCK;
	for (int i = 0; i < blockNum; i++)
	{
		BYTE* in = src + i * BF_BLOCK;
		BYTE* out = dst + i * BF_BLOCK;
		BF_ecb_encrypt(in, out, &key, BF_DECRYPT);
	}
}

void DecryptItem(string path,string out)
{
	string kBuff;
	if (RWFile::LoadFile(path, kBuff))
	{
		string kBuffBak = kBuff;
		int len = kBuff.length();
		BYTE* data = (BYTE*)kBuff.c_str();
		DecMir3ResDat(data, len);
		RWFile::SaveFile(out, kBuff);
		printf("DecMirDatRes OK!\n");
	}
}


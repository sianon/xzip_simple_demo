// xzip_demo.cpp : Defines the entry point for the console application.
//
#include<Windows.h>
#include "XZip.h"
#include "XUnzip.h"
#include <io.h>
#include <filesystem>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS
namespace fs = std::tr2::sys;
using namespace std;
using namespace fs;

int main()
{
	//压缩过程
	HZIP hz = CreateZip(0, 256 * 1024, ZIP_MEMORY);
	if (!hz) return false;

	fs::path full_path = fs::system_complete(fs::path("skin"));
	if (!fs::exists(full_path) || !fs::is_directory(full_path))
		return false;
	std::string path = full_path.string();
	fs::directory_iterator iter(full_path), end;
	while (iter != end) {
		fs::path item = iter->path();
		if (item.extension() == ".xml"
			&& item.filename() != "configc"
			&& item.filename() != "configp"
			&& item.filename() != "configs") {

			wstring xx = item.filename();
			wstring filepath = item.wstring().c_str();
			ZipAdd(hz, xx.c_str(), (void*)item.wstring().c_str(), 0, ZIP_FILENAME);
			wprintf(L"current file name %s \n", xx.c_str());
		}
		iter++;
	}
	unsigned long len = 0;
	PBYTE buf = 0;

	const fs::path result_file_path = full_path.remove_filename().append("default.x");
	if (ZipGetMemory(hz, (void**)&buf, &len) == ZR_OK) {
		FILE* file = _wfopen(result_file_path.wstring().c_str(), TEXT("wb"));
		fwrite(buf, sizeof(BYTE), len, file);
		fclose(file);
	}
	CloseZip(hz);

	//解压过程
	fs::path result_file_path = full_path.parent_path().wstring().append(L"\\default.x");
	HZIP hz_unzip = OpenZip((void*)result_file_path.wstring().c_str(), 0, ZIP_FILENAME);
	if (!hz_unzip) return false;

	ZIPENTRYW ze = { 0 };
	GetZipItem(hz_unzip, -1, &ze);
	int count = ze.index;
	for (int i = 0; i < count; i++) {
		fs::path unzip_path = full_path;
		GetZipItem(hz_unzip, i, &ze);
		UnzipItem(hz_unzip, i, (void*)(unzip_path.append(ze.name).c_str()), 0, ZIP_FILENAME);
		printf("%s.\n", unzip_path.string().c_str());
	}

	CloseZip(hz_unzip);
    return 0;
}


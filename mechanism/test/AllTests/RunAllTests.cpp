#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

void runTest(const TCHAR* file)
{
	if (_tcscmp(_T("AllTests.exe"), file) != 0) {
		_tprintf(_T("%s\n"), file);
		_tsystem(file);
	}
}

int main(void)
{
	WIN32_FIND_DATA findData;
	HANDLE h = ::FindFirstFile(_T("*Test.exe"), &findData);
	if (h == INVALID_HANDLE_VALUE) {
		_tprintf(_T("Not Found\n"));
		return -1;
	}
	runTest(findData.cFileName);
	while (::FindNextFile(h, &findData)) {
		runTest(findData.cFileName);
	}
	return 0;
}

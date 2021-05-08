#pragma once
#include "pch.h"
DWORD GetProcessId(const char* ProcessName)
{
	USES_CONVERSION;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	if (!Process32First(hSnap, &pe32))
		return NULL;

	do {

		if (!strcmp(W2A(pe32.szExeFile), ProcessName))
		{
			CloseHandle(hSnap);
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hSnap, &pe32));

	CloseHandle(hSnap);
	return NULL;
}
// Barak Gonen 2019
// Skeleton code - inject DLL to a running process
#include "pch.h"
#include "Functions.h"
#define PROGRAM_NAME "notepad.exe"
#define FUNCTION_ADRESS "LoadLibraryA"
#define KERNEL_DLL L"kernel32.dll"
#define DLL_NAME "DLL.dll"
int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE); //Used to not show the console
	LPSTR path = new CHAR[MAX_PATH];
	DWORD err, processID;
	DWORD pathLen = GetFullPathNameA(DLL_NAME, MAX_PATH, path, NULL); // Get full path of DLL to inject
	// Get LoadLibrary function address - the address doesn't change at remote process
	PVOID addrLoadLibrary = (PVOID)GetProcAddress(GetModuleHandle(KERNEL_DLL), FUNCTION_ADRESS);
	DWORD checkID = GetProcessId(PROGRAM_NAME);
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, checkID); // Open remote process
	// Get a pointer to memory location in remote process, big enough to store DLL path
	PVOID memAddr = (PVOID)VirtualAllocEx(proc, 0, pathLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NULL == memAddr) {
		err = GetLastError();
		return 0;
	}
	BOOL check = WriteProcessMemory(proc, memAddr, path, pathLen, NULL); // Write DLL name to remote process memory
	if (0 == check) {
		err = GetLastError();
		return 0;
	}
	// Open remote thread, while executing LoadLibrary with parameter DLL name, will trigger DLLMain
	HANDLE hRemote = CreateRemoteThread(proc, NULL, 0, LPTHREAD_START_ROUTINE(addrLoadLibrary), memAddr, NULL, NULL);
	if (NULL == hRemote) {
		err = GetLastError();
		return 0;
	}
	WaitForSingleObject(hRemote, INFINITE); // Waits until the specified object is in the signaled state or the time-out interval elapses.
	VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE); // Free the memory location in remote process
	check = CloseHandle(hRemote); //Closes an open object handle - the thread 
	check = CloseHandle(proc); //Closes an open object handle - the process 
	delete[] path;
	return 1;
}
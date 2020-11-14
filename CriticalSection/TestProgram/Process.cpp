#include <iostream>
#include <windows.h>
#include <string>

TCHAR syncName[] = TEXT("CriticalSection");
CRITICAL_SECTION* critsect;

int main()
{
	
	HANDLE fileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, syncName);

	if (fileMapping == NULL) return -1;

	critsect = (CRITICAL_SECTION*)MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CRITICAL_SECTION));

	if (critsect == NULL)
	{
		CloseHandle(fileMapping);
		return -2;
	}
	
	int procId = GetCurrentProcessId();

	while (!TryEnterCriticalSection(critsect)) Sleep(10);

	std::cout << "Process " << procId << " enter" << std::endl;
	std::cout << "Process " << procId << " work" << std::endl;
	std::cout << "Process " << procId << " exit" << std::endl;

	LeaveCriticalSection(critsect);
	UnmapViewOfFile(critsect);
	CloseHandle(fileMapping);

	return 0;
}
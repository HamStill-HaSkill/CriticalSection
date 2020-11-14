#include <iostream>
#include <windows.h>
#include <memoryapi.h>
#include <string>

#define MAX_PROCESSES 5

TCHAR syncName[] = TEXT("CriticalSection");
CRITICAL_SECTION* critsect;

std::wstring GetExePath()
{
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

void createProcesses()
{
	STARTUPINFO sturtupInfos[MAX_PROCESSES];
	PROCESS_INFORMATION processInformations[MAX_PROCESSES];

	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		ZeroMemory(&sturtupInfos[i], sizeof(STARTUPINFO));
		ZeroMemory(&processInformations[i], sizeof(PROCESS_INFORMATION));
		sturtupInfos[i].cb = sizeof(STARTUPINFO);

		if (!CreateProcess(std::wstring(GetExePath() + L"\\Process.exe").c_str(), NULL, 0, 0, TRUE, 0, 0, NULL, &sturtupInfos[i], &processInformations[i]))
		{
			std::cout << "Failed to create process Error:" << GetLastError() << std::endl;
		}
	}

	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		DWORD res = WaitForSingleObject(processInformations[i].hProcess, INFINITE);
		CloseHandle(processInformations[i].hProcess);
		CloseHandle(processInformations[i].hThread);
	}
}

int main()
{
	HANDLE fileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CRITICAL_SECTION), syncName);

	if (fileMapping == NULL) return -1;

	critsect = (CRITICAL_SECTION*)MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CRITICAL_SECTION));

	if (critsect == NULL)
	{
		CloseHandle(fileMapping);
		return -2;
	}

	InitializeCriticalSection(critsect);

	createProcesses();

	DeleteCriticalSection(critsect);
	UnmapViewOfFile(critsect);
	CloseHandle(fileMapping);

	return 0;
}
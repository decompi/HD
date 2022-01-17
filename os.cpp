#include "os.h"

const char* path_1 = "SYSTEM\\CurrentControlSet\\Control\\SystemInformation";
HW_PROFILE_INFO hardware;
SYSTEM_INFO siSysInfo;

void toLowerCase(char* ptr, size_t size)
{
	for (uint32_t i = 0; i < size; i++) {
		if (isupper(ptr[i]))
			ptr[i] = tolower(ptr[i]);
	}
}
// CPU NAME
std::string OperatingSystem::GetCPU() {
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int Ids = CPUInfo[0];
	char szCPU[0x40] = { 0 };

	for (unsigned int i = 0x80000000; i <= Ids; ++i)
	{
		__cpuid(CPUInfo, i);
		int size = sizeof(CPUInfo);

		if (i == 0x80000002)
			memcpy(szCPU, CPUInfo, size);
		else if (i == 0x80000003)
			memcpy(szCPU + 16, CPUInfo, size);
		else if (i == 0x80000004)
			memcpy(szCPU + 32, CPUInfo, size);
	}
	return szCPU;
}
// COMPUTER NAME
std::string OperatingSystem::GetCN() {
	GetSystemInfo(&siSysInfo);

	std::string szProcCount = std::to_string(siSysInfo.dwNumberOfProcessors);
	char szComputerName[1024];
	DWORD dwSize = sizeof(szComputerName);
	GetComputerNameA(szComputerName, &dwSize);

	return szComputerName;
}
// COMPUTER USER NAME
std::string OperatingSystem::GetComputerUserName() {
	GetSystemInfo(&siSysInfo);
	char szUsername[1024];
	DWORD dwUser = sizeof(szUsername);
	GetUserNameA(szUsername, &dwUser);

	return szUsername;
}
// COMPUTER MANUFACTURER
std::string OperatingSystem::GetManufacturer() {
	GetSystemInfo(&siSysInfo);

	char buf[1000];
	int ret;
	DWORD sz = 1000;
	ret = RegGetValueA(HKEY_LOCAL_MACHINE, path_1, OBFUSCATE("SystemManufacturer"), RRF_RT_ANY, NULL, &buf[0], &sz);
	toLowerCase(buf, strlen(buf));
	return buf;
}
// SYSTEM NAME
std::string OperatingSystem::GetSystemName() {
	GetSystemInfo(&siSysInfo);

	char buf[1000];
	int ret;
	DWORD sz = 1000;

	ret = RegGetValueA(HKEY_LOCAL_MACHINE, path_1, OBFUSCATE("SystemProductName"), RRF_RT_ANY, NULL, &buf[0], &sz);
	toLowerCase(buf, strlen(buf));
	return buf;
}
// HWID
std::string OperatingSystem::GetHWID() {
	if (GetCurrentHwProfile(&hardware)) {
		std::wstring ws(hardware.szHwProfileGuid);
		std::string hwid(ws.begin(), ws.end());
		return hwid;
	}
	else {
		return "failed";
	}
}
